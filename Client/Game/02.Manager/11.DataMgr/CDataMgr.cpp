#include "pch.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include "Game\02.Manager\06.PathMgr\CPathMgr.h"

#include <cerrno>
#include <cmath>

namespace
{
    constexpr const wchar_t* ERROR_TITLE = L"데이터 파일 오류";

    wstring ShowName(const wstring& _strFile)
    {
        return L"data\\" + _strFile + L".ini";
    }

    void Report(const wstring& _strMsg)
    {
        ReportError(ERROR_TITLE, _strMsg.c_str());
    }

    wstring Trim(const wstring& _str)
    {
        const size_t iBegin = _str.find_first_not_of(L" \t\r");
        if (wstring::npos == iBegin)
            return wstring();
        const size_t iEnd = _str.find_last_not_of(L" \t\r");
        return _str.substr(iBegin, iEnd - iBegin + 1);
    }

    // 쉼표로 나눈 칸들 (칸마다 앞뒤 공백을 뺀다)
    vector<wstring> SplitComma(const wstring& _str)
    {
        vector<wstring> vecOut;
        size_t          iFrom = 0;
        while (true)
        {
            const size_t iComma = _str.find(L',', iFrom);
            vecOut.push_back(Trim(_str.substr(iFrom, wstring::npos == iComma ? wstring::npos : iComma - iFrom)));
            if (wstring::npos == iComma)
                break;
            iFrom = iComma + 1;
        }
        return vecOut;
    }

    // 글자 전체가 숫자 하나일 때만 읽는다 ("3.5f"·"12px"·빈 칸은 틀린 값)
    bool ToFloat(const wstring& _str, float& _fOut)
    {
        wchar_t* pEnd = nullptr;
        errno         = 0;
        const float f = wcstof(_str.c_str(), &pEnd);
        if (_str.empty() || pEnd == _str.c_str() || L'\0' != *pEnd || ERANGE == errno || !std::isfinite(f))
            return false;
        _fOut = f;
        return true;
    }

    bool ToDouble(const wstring& _str, double& _dOut)
    {
        wchar_t* pEnd  = nullptr;
        errno          = 0;
        const double d = wcstod(_str.c_str(), &pEnd);
        if (_str.empty() || pEnd == _str.c_str() || L'\0' != *pEnd || ERANGE == errno || !std::isfinite(d))
            return false;
        _dOut = d;
        return true;
    }

    bool ToInt(const wstring& _str, int& _iOut)
    {
        wchar_t*   pEnd = nullptr;
        errno           = 0;
        const long i    = wcstol(_str.c_str(), &pEnd, 10);
        if (_str.empty() || pEnd == _str.c_str() || L'\0' != *pEnd || ERANGE == errno)
            return false;
        _iOut = static_cast<int>(i);
        return true;
    }
}

float CDataSection::GetFloat(const wchar_t* _szKey) const
{
    float          fValue = 0.f;
    const wstring* pValue = Find(_szKey);
    if (nullptr != pValue && !ToFloat(*pValue, fValue))
        ReportBad(_szKey, (L"숫자가 아닙니다: " + *pValue).c_str());
    return fValue;
}

double CDataSection::GetDouble(const wchar_t* _szKey) const
{
    double         dValue = 0.0;
    const wstring* pValue = Find(_szKey);
    if (nullptr != pValue && !ToDouble(*pValue, dValue))
        ReportBad(_szKey, (L"숫자가 아닙니다: " + *pValue).c_str());
    return dValue;
}

int CDataSection::GetInt(const wchar_t* _szKey) const
{
    int            iValue = 0;
    const wstring* pValue = Find(_szKey);
    if (nullptr != pValue && !ToInt(*pValue, iValue))
        ReportBad(_szKey, (L"정수가 아닙니다: " + *pValue).c_str());
    return iValue;
}

Vec2 CDataSection::GetVec2(const wchar_t* _szKey) const
{
    const wstring* pValue = Find(_szKey);
    if (nullptr == pValue)
        return Vec2();

    const vector<wstring> vecPart = SplitComma(*pValue);
    Vec2                  vValue;
    if (2 != vecPart.size() || !ToFloat(vecPart[0], vValue.x) || !ToFloat(vecPart[1], vValue.y))
    {
        ReportBad(_szKey, (L"\"x, y\" 두 숫자가 아닙니다: " + *pValue).c_str());
        return Vec2();
    }
    return vValue;
}

COLORREF CDataSection::GetColor(const wchar_t* _szKey) const
{
    const wstring* pValue = Find(_szKey);
    if (nullptr == pValue)
        return RGB(0, 0, 0);

    const vector<wstring> vecPart = SplitComma(*pValue);
    int                   arrRGB[3] = {};
    bool                  bOk       = 3 == vecPart.size();
    for (size_t i = 0; bOk && i < 3; ++i)
        bOk = ToInt(vecPart[i], arrRGB[i]) && 0 <= arrRGB[i] && arrRGB[i] <= 255;
    if (!bOk)
    {
        ReportBad(_szKey, (L"\"r, g, b\" (0~255) 세 정수가 아닙니다: " + *pValue).c_str());
        return RGB(0, 0, 0);
    }
    return RGB(arrRGB[0], arrRGB[1], arrRGB[2]);
}

const wstring* CDataSection::Find(const wchar_t* _szKey) const
{
    const auto iter = m_mapValue.find(_szKey);
    if (iter != m_mapValue.end())
        return &iter->second;

    // 파일이나 구역이 없으면 그것만 한 번 알렸다
    if (!m_bMissing)
        ReportBad(_szKey, L"값이 없습니다");
    return nullptr;
}

void CDataSection::ReportBad(const wchar_t* _szKey, const wchar_t* _szWhy) const
{
    // 같은 키를 여러 오브젝트가 읽어도 한 번만 알린다
    if (!m_setReported.insert(_szKey).second)
        return;
    Report(ShowName(m_strFile) + L" [" + m_strName + L"] " + _szKey + L": " + _szWhy);
}

CDataMgr::CDataMgr()
{
}

CDataMgr::~CDataMgr()
{
}

void CDataMgr::init()
{
    // 화면·그림 배율은 오브젝트와 레벨 곳곳이 쓰므로 실행할 때 한 번만 읽는다
    const CDataSection& tScale = GetSection(L"game", L"scale");
    m_fBgMagni                 = tScale.GetFloat(L"background");
    m_fAnimMagni               = tScale.GetFloat(L"animation");
}

void CDataMgr::Reload()
{
    m_mapFile.clear();
    m_setMissingFile.clear();
}

const CDataSection& CDataMgr::GetSection(const wchar_t* _szFile, const wchar_t* _szSection)
{
    map<wstring, CDataSection>& mapSection = LoadFile(_szFile);
    const auto                  iter       = mapSection.find(_szSection);
    if (iter != mapSection.end())
        return iter->second;

    // 없는 구역은 한 번만 알리고, 값을 물으면 조용히 0을 돌려주는 빈 구역을 둔다
    if (0 == m_setMissingFile.count(_szFile))
        Report(ShowName(_szFile) + L"에 [" + _szSection + L"] 구역이 없습니다");

    CDataSection& tSection = mapSection[_szSection];
    tSection.m_strFile     = _szFile;
    tSection.m_strName     = _szSection;
    tSection.m_bMissing    = true;
    return tSection;
}

void CDataMgr::SetOverride(const wstring& _strFile, const wstring& _strSection, const wstring& _strKey, const wstring& _strValue)
{
    m_vecOverride.push_back(tOverride{ _strFile, _strSection, _strKey, _strValue });
    Reload();
}

map<wstring, CDataSection>& CDataMgr::LoadFile(const wstring& _strFile)
{
    const auto iter = m_mapFile.find(_strFile);
    if (iter != m_mapFile.end())
        return iter->second;

    map<wstring, CDataSection>& mapSection = m_mapFile[_strFile];
    const wstring               strShow    = ShowName(_strFile);
    const wstring               strPath    = wstring(CPathMgr::GetInst()->GetContentPath()) + strShow;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"rb");
    if (nullptr == pFile)
    {
        Report(strShow + L" 파일을 열 수 없습니다");
        m_setMissingFile.insert(_strFile);
        return mapSection;
    }

    string strBytes;
    fseek(pFile, 0, SEEK_END);
    const long iBytes = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    if (iBytes > 0)
    {
        strBytes.resize(static_cast<size_t>(iBytes));
        strBytes.resize(fread(&strBytes[0], 1, strBytes.size(), pFile));
    }
    fclose(pFile);

    // UTF-8 (BOM이 있어도 된다)
    if (strBytes.size() >= 3 && 0 == strBytes.compare(0, 3, "\xEF\xBB\xBF"))
        strBytes.erase(0, 3);

    wstring strText;
    if (!strBytes.empty())
    {
        const int iBytesIn = static_cast<int>(strBytes.size());
        const int iLen     = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, strBytes.data(), iBytesIn, nullptr, 0);
        if (iLen <= 0)
        {
            Report(strShow + L" 파일이 UTF-8이 아닙니다");
            m_setMissingFile.insert(_strFile);
            return mapSection;
        }
        strText.resize(static_cast<size_t>(iLen));
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, strBytes.data(), iBytesIn, &strText[0], iLen);
    }

    // 한 줄에 하나: [구역] 또는 키 = 값. ; 나 # 뒤는 주석
    CDataSection* pSection = nullptr;
    int           iLine    = 0;
    size_t        iFrom    = 0;
    while (iFrom <= strText.size())
    {
        size_t iEnd = strText.find(L'\n', iFrom);
        if (wstring::npos == iEnd)
            iEnd = strText.size();
        wstring strLine = strText.substr(iFrom, iEnd - iFrom);
        iFrom           = iEnd + 1;
        ++iLine;

        const size_t iComment = strLine.find_first_of(L";#");
        if (wstring::npos != iComment)
            strLine.erase(iComment);
        strLine = Trim(strLine);
        if (strLine.empty())
            continue;

        const wstring strWhere = strShow + L" " + std::to_wstring(iLine) + L"번째 줄: ";
        if (L'[' == strLine.front())
        {
            const wstring strName = L']' == strLine.back() ? Trim(strLine.substr(1, strLine.size() - 2)) : wstring();
            if (strName.empty())
            {
                Report(strWhere + L"구역 이름은 [이름] 으로 씁니다");
                pSection = nullptr;
                continue;
            }
            if (0 != mapSection.count(strName))
                Report(strWhere + L"[" + strName + L"] 구역이 두 번 있습니다");
            pSection            = &mapSection[strName];
            pSection->m_strFile = _strFile;
            pSection->m_strName = strName;
            continue;
        }

        const size_t  iEqual = strLine.find(L'=');
        const wstring strKey = wstring::npos == iEqual ? wstring() : Trim(strLine.substr(0, iEqual));
        if (strKey.empty())
        {
            Report(strWhere + L"\"키 = 값\" 형식이 아닙니다");
            continue;
        }
        if (nullptr == pSection)
        {
            Report(strWhere + L"[구역] 앞에 값이 있습니다");
            continue;
        }
        if (!pSection->m_mapValue.emplace(strKey, Trim(strLine.substr(iEqual + 1))).second)
            Report(strWhere + L"[" + pSection->m_strName + L"] " + strKey + L"가 두 번 있습니다");
    }

    // 자동 테스트가 바꿔 쓰는 값. 파일에 없는 키면 시나리오의 오타다
    for (const tOverride& tOver : m_vecOverride)
    {
        if (tOver.strFile != _strFile)
            continue;
        const auto iterSection = mapSection.find(tOver.strSection);
        if (iterSection == mapSection.end() || 0 == iterSection->second.m_mapValue.count(tOver.strKey))
        {
            Report(L"자동 테스트가 바꾸려는 " + strShow + L" [" + tOver.strSection + L"] " + tOver.strKey + L"가 파일에 없습니다");
            continue;
        }
        iterSection->second.m_mapValue[tOver.strKey] = tOver.strValue;
    }

    return mapSection;
}
