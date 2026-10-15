#include "pch.h"
#include "Game\06.Resource\Anim\CAnimData.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include <cwctype>

namespace
{
    constexpr int MAX_FRAME_COUNT = 10000;   // 이보다 많다고 적힌 파일은 깨진 것으로 본다

    // 공백으로 나눈 토큰을 앞에서부터 하나씩 꺼낸다
    class CTokenReader
    {
    private:
        const wstring& m_strText;
        size_t         m_iPos = 0;

    public:
        bool Next(wstring& _strOut)
        {
            while (m_iPos < m_strText.size() && iswspace(m_strText[m_iPos]))
                ++m_iPos;
            if (m_iPos >= m_strText.size())
                return false;

            const size_t iStart = m_iPos;
            while (m_iPos < m_strText.size() && !iswspace(m_strText[m_iPos]))
                ++m_iPos;
            _strOut = m_strText.substr(iStart, m_iPos - iStart);
            return true;
        }

        explicit CTokenReader(const wstring& _strText) : m_strText(_strText) {}
    };

    bool ToFloat(const wstring& _str, float& _fOut)
    {
        wchar_t* pEnd = nullptr;
        _fOut         = wcstof(_str.c_str(), &pEnd);
        return pEnd != _str.c_str() && L'\0' == *pEnd;
    }

    bool ToInt(const wstring& _str, int& _iOut)
    {
        wchar_t*   pEnd   = nullptr;
        const long iValue = wcstol(_str.c_str(), &pEnd, 10);
        _iOut             = static_cast<int>(iValue);
        return pEnd != _str.c_str() && L'\0' == *pEnd;
    }

    bool HasSpace(const wstring& _str)
    {
        for (const wchar_t ch : _str)
        {
            if (iswspace(ch))
                return true;
        }
        return false;
    }

    // 예전 저장 코드의 fwprintf_s(L"%.1f %.1f\n") 와 같은 글자
    void AppendVec2(wstring& _str, const Vec2& _v)
    {
        wchar_t szBuf[128] = {};
        swprintf_s(szBuf, L"%.1f %.1f\n", _v.x, _v.y);
        _str += szBuf;
    }
}

CAnimData::CAnimData()
    : m_pAtlas(nullptr)
{
}

CAnimData::~CAnimData()
{
}

bool CAnimData::ReadFile(const wstring& _strFullPath, tAnimFile& _tOut, wstring& _strError)
{
    _tOut = {};
    _strError.clear();

    auto Fail = [&](const wstring& _strWhy)
    {
        _strError = _strWhy + L"\n" + _strFullPath;
        return false;
    };

    // 파일 전체를 넓은 문자로 읽는다 (UTF-16LE. BOM이 있으면 건너뛴다)
    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, _strFullPath.c_str(), L"rb");
    if (nullptr == pFile)
        return Fail(L"파일을 열 수 없습니다 (Error Number : " + std::to_wstring(iErrNum) + L")");

    fseek(pFile, 0, SEEK_END);
    const long iBytes = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    wstring strText;
    if (iBytes > 0)
    {
        strText.resize(static_cast<size_t>(iBytes) / sizeof(wchar_t));
        strText.resize(fread(&strText[0], sizeof(wchar_t), strText.size(), pFile));
    }
    fclose(pFile);
    if (!strText.empty() && 0xFEFF == strText[0])
        strText.erase(0, 1);

    // 태그 다음 토큰이 값이다. 프레임은 [LEFT_TOP]·[SIZE]·[OFFSET]·[DURATION] 순서로 [DURATION]에서 끝나고,
    // [0_FRAME] 같은 다른 토큰은 건너뛴다 (예전 읽기 코드와 같은 규칙)
    CTokenReader tReader(strText);
    wstring      strTok;
    auto         ReadVec2 = [&](Vec2& _v)
    {
        wstring strX, strY;
        return tReader.Next(strX) && tReader.Next(strY) && ToFloat(strX, _v.x) && ToFloat(strY, _v.y);
    };

    while (tReader.Next(strTok))
    {
        if (L"[ANIMATION_NAME]" == strTok)
        {
            if (!tReader.Next(_tOut.strName))
                return Fail(L"[ANIMATION_NAME] 값이 없습니다");
        }
        else if (L"[ATLAS_KEY]" == strTok)
        {
            if (!tReader.Next(_tOut.strAtlasKey))
                return Fail(L"[ATLAS_KEY] 값이 없습니다");
        }
        else if (L"[ATLAS_PATH]" == strTok)
        {
            if (!tReader.Next(_tOut.strAtlasPath))
                return Fail(L"[ATLAS_PATH] 값이 없습니다");
        }
        else if (L"[FRAME_COUNT]" == strTok)
        {
            wstring strCount;
            int     iCount = -1;
            if (!tReader.Next(strCount) || !ToInt(strCount, iCount) || iCount < 0 || iCount > MAX_FRAME_COUNT)
                return Fail(L"[FRAME_COUNT] 값이 잘못됐습니다 : " + strCount);

            _tOut.vecFrm.reserve(static_cast<size_t>(iCount));
            for (int i = 0; i < iCount; ++i)
            {
                tAnimFrm frm       = {};
                bool     bDuration = false;
                while (!bDuration && tReader.Next(strTok))
                {
                    bool bOk = true;
                    if (L"[LEFT_TOP]" == strTok)
                        bOk = ReadVec2(frm.vLeftTop);
                    else if (L"[SIZE]" == strTok)
                        bOk = ReadVec2(frm.vSize);
                    else if (L"[OFFSET]" == strTok)
                        bOk = ReadVec2(frm.vOffset);
                    else if (L"[DURATION]" == strTok)
                    {
                        wstring strValue;
                        bOk       = tReader.Next(strValue) && ToFloat(strValue, frm.fDuration);
                        bDuration = true;
                    }

                    if (!bOk)
                        return Fail(std::to_wstring(i) + L"번 프레임의 " + strTok + L" 값이 잘못됐습니다");
                }

                // 예전에는 개수만큼 다 읽지 못해도 빈 프레임을 채우거나(엔진) 끝없이 읽었다(에디터)
                if (!bDuration)
                    return Fail(L"프레임이 " + std::to_wstring(iCount) + L"개라고 적혀 있는데 " + std::to_wstring(i) + L"개만 있습니다");
                _tOut.vecFrm.push_back(frm);
            }
            return true;
        }
    }

    return Fail(L"[FRAME_COUNT]가 없습니다 (빈 파일이거나 .anim 파일이 아닙니다)");
}

bool CAnimData::WriteFile(const wstring& _strFullPath, const tAnimFile& _tIn, wstring& _strError)
{
    _strError.clear();

    // 빈 애니메이션으로 덮어쓰지 않는다 (예전 에디터는 파일을 먼저 비운 뒤 애니메이션이 없어 크래시했다)
    if (_tIn.vecFrm.empty())
    {
        _strError = L"저장할 프레임이 없습니다";
        return false;
    }

    // 읽는 쪽이 공백으로 토큰을 나누므로 공백이 든 값은 다시 읽을 수 없다
    const std::pair<const wchar_t*, const wstring*> arrValue[] =
    {
        { L"애니메이션 이름", &_tIn.strName },
        { L"아틀라스 키", &_tIn.strAtlasKey },
        { L"아틀라스 경로", &_tIn.strAtlasPath },
    };
    for (const auto& tValue : arrValue)
    {
        if (tValue.second->empty() || HasSpace(*tValue.second))
        {
            _strError = wstring(tValue.first) + L"이 비었거나 공백이 있습니다 : \"" + *tValue.second + L"\"";
            return false;
        }
    }

    // 예전 저장 코드(fwprintf_s, "wb")와 같은 글자를 만든다. 지금 있는 .anim 123개가 모두 한 글자도 다르지 않게 다시 써진다
    wstring strText = L"\n[ANIMATION_NAME]\n" + _tIn.strName + L"\n\n"
                    + L"[ATLAS_KEY]\n" + _tIn.strAtlasKey + L"\n\n"
                    + L"[ATLAS_PATH]\n" + _tIn.strAtlasPath + L"\n\n"
                    + L"[FRAME_COUNT]\n" + std::to_wstring(_tIn.vecFrm.size()) + L"\n\n";
    for (size_t i = 0; i < _tIn.vecFrm.size(); ++i)
    {
        const tAnimFrm& frm = _tIn.vecFrm[i];
        strText += L"[" + std::to_wstring(i) + L"_FRAME]\n";
        strText += L"[LEFT_TOP]\n";
        AppendVec2(strText, frm.vLeftTop);
        strText += L"[SIZE]\n";
        AppendVec2(strText, frm.vSize);
        strText += L"[OFFSET]\n";
        AppendVec2(strText, frm.vOffset);

        wchar_t szDuration[64] = {};
        swprintf_s(szDuration, L"[DURATION]\n%.2f\n\n", frm.fDuration);
        strText += szDuration;
    }

    const wstring strTemp = _strFullPath + L".tmp";
    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strTemp.c_str(), L"wb");
    if (nullptr == pFile)
    {
        _strError = L"파일을 만들 수 없습니다 (Error Number : " + std::to_wstring(iErrNum) + L")\n" + strTemp;
        return false;
    }
    const bool bWritten = strText.size() == fwrite(strText.data(), sizeof(wchar_t), strText.size(), pFile);
    const bool bClosed  = 0 == fclose(pFile);
    if (!bWritten || !bClosed || !MoveFileEx(strTemp.c_str(), _strFullPath.c_str(), MOVEFILE_REPLACE_EXISTING))
    {
        DeleteFile(strTemp.c_str());
        _strError = L"파일을 쓰지 못했습니다\n" + _strFullPath;
        return false;
    }
    return true;
}

int CAnimData::Load(const wstring& _strFilePath)
{
    tAnimFile tFile;
    wstring   strError;
    if (!ReadFile(_strFilePath, tFile, strError))
    {
        ReportError(L"Animation Load 실패", strError.c_str());
        return E_FAIL;
    }

    m_strName = tFile.strName;
    m_vecFrm  = std::move(tFile.vecFrm);
    if (!tFile.strAtlasKey.empty())
        m_pAtlas = CResMgr::GetInst()->LoadTexture(tFile.strAtlasKey, tFile.strAtlasPath);

    // 프레임이 없는 애니메이션은 재생·그리기를 건너뛴다. 원인을 알 수 있게 알린다
    if (m_vecFrm.empty())
    {
        const wstring strMsg = L"프레임이 없는 애니메이션 : " + _strFilePath;
        ReportError(L"Animation Load 실패", strMsg.c_str());
        return E_FAIL;
    }
    return S_OK;
}
