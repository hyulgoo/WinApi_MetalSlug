#include "pch.h"
#include "Game\06.Resource\Anim\CAnimData.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"

CAnimData::CAnimData()
    : m_pAtlas(nullptr)
{
}

CAnimData::~CAnimData()
{
}

int CAnimData::Load(const wstring& _strFilePath)
{
    // 파일은 UTF-16. 이진 모드로 열어 넓은 문자 단위로 읽는다 (에디터의 저장과 같은 방식)
    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
    if (nullptr == pFile)
    {
        wchar_t szStr[512] = {};
        swprintf_s(szStr, L"Animation Load 실패, Error Number : %d\n%s", iErrNum, _strFilePath.c_str());
        ReportError(L"파일 로드 실패", szStr);
        return E_FAIL;
    }

    size_t  iFrameCount = 0;
    wstring strAtlasKey;
    wstring strAtlasRelativePath;

    while (true)
    {
        wchar_t szBuffer[256] = {};
        if (1 != fwscanf_s(pFile, L"%s", szBuffer, 256))
            break;

        if (!wcscmp(szBuffer, L"[ANIMATION_NAME]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            m_strName = szBuffer;
        }
        else if (!wcscmp(szBuffer, L"[ATLAS_KEY]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            strAtlasKey = szBuffer;
        }
        else if (!wcscmp(szBuffer, L"[ATLAS_PATH]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            strAtlasRelativePath = szBuffer;
        }
        else if (!wcscmp(szBuffer, L"[FRAME_COUNT]"))
        {
            fwscanf_s(pFile, L"%zd", &iFrameCount);

            for (size_t i = 0; i < iFrameCount; ++i)
            {
                tAnimFrm frm = {};

                while (true)
                {
                    wchar_t szToken[256] = {};
                    if (1 != fwscanf_s(pFile, L"%s", szToken, 256))
                        break;

                    if (!wcscmp(szToken, L"[LEFT_TOP]"))
                        fwscanf_s(pFile, L"%f %f", &frm.vLeftTop.x, &frm.vLeftTop.y);
                    else if (!wcscmp(szToken, L"[SIZE]"))
                        fwscanf_s(pFile, L"%f %f", &frm.vSize.x, &frm.vSize.y);
                    else if (!wcscmp(szToken, L"[OFFSET]"))
                        fwscanf_s(pFile, L"%f %f", &frm.vOffset.x, &frm.vOffset.y);
                    else if (!wcscmp(szToken, L"[DURATION]"))
                    {
                        fwscanf_s(pFile, L"%f", &frm.fDuration);
                        break;
                    }
                }
                m_vecFrm.push_back(frm);
            }
            break;
        }
    }

    fclose(pFile);

    if (!strAtlasKey.empty())
        m_pAtlas = CResMgr::GetInst()->LoadTexture(strAtlasKey, strAtlasRelativePath);

    // 프레임이 없는 애니메이션은 재생·그리기를 건너뛴다. 원인을 알 수 있게 알린다
    if (m_vecFrm.empty())
    {
        const wstring strMsg = L"프레임이 없는 애니메이션 : " + _strFilePath;
        ReportError(L"Animation Load 실패", strMsg.c_str());
        return E_FAIL;
    }
    return S_OK;
}
