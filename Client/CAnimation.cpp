#include "pch.h"
#include "CAnimation.h"

#include "CObj.h"
#include "CAnimator.h"
#include "CTexture.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CEngine.h"
#include <cmath>


CAnimation::CAnimation(CAnimator* _pAnimator)
    : m_pAnimator(_pAnimator)
    , m_pAtlas(nullptr)
    , m_iCurFrm(0)
    , m_fAccTime(0.f)
    , m_bFinish(false)
    , m_pBackBuffer(nullptr)
    , m_fmagni(3.5)
    , m_bReverse(false)
    , m_vOffset{}
    , m_tPrintMode(SETPRINT::NORMAL)
{
    // 애니메이션용 백버퍼 미리 생성, 키 컬러(마젠타)로 채워둠
    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    m_pBackBuffer          = CResMgr::GetInst()->CreateTexture(L"AnimBackBuffer", static_cast<int>(vResolution.x), static_cast<int>(vResolution.y));
    SetDCBrushColor(m_pBackBuffer->GetDC(), RGB(255, 0, 255));
    Rectangle(m_pBackBuffer->GetDC(), -1, -1, static_cast<int>(vResolution.x) + 1, static_cast<int>(vResolution.y) + 1);
}

CAnimation::~CAnimation()
{
}

void CAnimation::init(const wstring& _strName, CTexture* _pAtlas, const Vec2& _vLeftTop, const Vec2& _vSize, const Vec2& _vOffset, const int _iMaxFrmCount, const float _fDuration)
{
    // 생성할 애니메이션 초기화 - 이름, 아틀라스 텍스처, 자를 시작위치, 사이즈, 재생시간 입력받아 세팅
    SetName(_strName);
    m_pAtlas = _pAtlas;

    for (int i = 0; i < _iMaxFrmCount; ++i)
    {
        tAnimFrm frm = {};

        frm.vLeftTop  = Vec2(_vLeftTop.x + static_cast<float>(i) * _vSize.x, _vLeftTop.y);
        frm.vSize     = _vSize;
        frm.vOffset   = _vOffset;
        frm.fDuration = _fDuration;
        m_vecFrm.push_back(frm);
    }
}

void CAnimation::tick()
{
    if (m_bFinish) // 이미 재생이 끝났다면 return
        return;
    m_fAccTime += DT;

    if (m_vecFrm[m_iCurFrm].fDuration < m_fAccTime) // AccTime이 현재 프레임의 지속시간을 넘으면 다음 프레임으로
    {
        ++m_iCurFrm;
        m_fAccTime = 0.f;

        if (m_vecFrm.size() <= m_iCurFrm) // 마지막 프레임과 같거나 커지면 마지막 프레임으로 고정하고
        {
            m_iCurFrm = static_cast<int>(m_vecFrm.size() - 1);
            m_bFinish = true;
        }
    }
}

void CAnimation::render(const HDC _dc)
{
    Vec2 vPos = {};
    vPos      = m_pAnimator->GetOwner()->GetPos();

    const tAnimFrm frm = m_vecFrm[m_iCurFrm];
    if (m_tPrintMode == SETPRINT::UPPER)
        m_vOffset = Vec2(-5.7f, -25.f);
    else if (m_tPrintMode == SETPRINT::LOWER)
        m_vOffset = Vec2(-5.7f, 0.f);
    else if (m_tPrintMode == SETPRINT::NORMAL)
        m_vOffset = {};

    if (m_bReverse == true)
    {
        StretchBlt(m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(frm.vSize.x * m_fmagni), static_cast<int>(frm.vSize.y * m_fmagni)
                 , m_pAtlas->GetDC()
                 , static_cast<int>(frm.vLeftTop.x + frm.vSize.x - 1)
                 , static_cast<int>(frm.vLeftTop.y)
                 , -static_cast<int>(frm.vSize.x)
                 , static_cast<int>(frm.vSize.y), SRCCOPY);
        TransparentBlt(_dc
                     , static_cast<int>(vPos.x - (frm.vOffset.x * m_fmagni) - (m_vOffset.x * m_fmagni) - (frm.vSize.x * m_fmagni))
                     , static_cast<int>(vPos.y + (frm.vOffset.y * m_fmagni) + (m_vOffset.y * m_fmagni))
                     , static_cast<int>(frm.vSize.x * m_fmagni)
                     , static_cast<int>(frm.vSize.y * m_fmagni)
                     , m_pBackBuffer->GetDC()
                     , 0, 0
                     , static_cast<int>(frm.vSize.x * m_fmagni), static_cast<int>(frm.vSize.y * m_fmagni)
                     , RGB(255, 0, 255));
    }
    else if (m_bReverse == false)
    {
        StretchBlt(m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(frm.vSize.x * m_fmagni), static_cast<int>(frm.vSize.y * m_fmagni)
                 , m_pAtlas->GetDC()
                 , static_cast<int>(frm.vLeftTop.x)
                 , static_cast<int>(frm.vLeftTop.y)
                 , static_cast<int>(frm.vSize.x)
                 , static_cast<int>(frm.vSize.y), SRCCOPY);
        TransparentBlt(_dc
                     , static_cast<int>(vPos.x + (frm.vOffset.x * m_fmagni) + (m_vOffset.x * m_fmagni))
                     , static_cast<int>(vPos.y + (frm.vOffset.y * m_fmagni) + (m_vOffset.y * m_fmagni))
                     , static_cast<int>(frm.vSize.x * m_fmagni)
                     , static_cast<int>(frm.vSize.y * m_fmagni)
                     , m_pBackBuffer->GetDC()
                     , 0, 0
                     , static_cast<int>(frm.vSize.x * m_fmagni), static_cast<int>(frm.vSize.y * m_fmagni)
                     , RGB(255, 0, 255));
    }
}

void CAnimation::NextFrm()
{
    if (m_iCurFrm == m_vecFrm.size() - 1)
        return;
    ++m_iCurFrm;
}

void CAnimation::PrevFrm()
{
    if (m_iCurFrm == 0)
        return;
    --m_iCurFrm;
}

void CAnimation::SetCurFrmInfo(const tAnimFrm& _Other)
{
    m_vecFrm[m_iCurFrm] = _Other;
}

void CAnimation::SetAllFrmInfo(const vector<tAnimFrm>& _Other)
{
    m_vecFrm = _Other;
}

// dc, 텍스처 좌상단, 자를 사이즈, 텍스처 dc, 찍을 위치, 자를 사이즈, 컬러키 지정
void CAnimation::Save(const wstring& _strRelativePath)
{
    wstring strFilepath = CPathMgr::GetInst()->GetContentPath();
    strFilepath         += _strRelativePath;

    // 파일 열기
    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strFilepath.c_str(), L"wb");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
        MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
        return;
    }

    // Animation 이름 저장
    fwprintf_s(pFile, L"\n");

    fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
    fwprintf_s(pFile, GetName().c_str());
    fwprintf_s(pFile, L"\n\n");

    // 아틀라스 텍스처 키값 저장
    fwprintf_s(pFile, L"[ATLAS_KEY]\n");
    fwprintf_s(pFile, m_pAtlas->GetKey().c_str());
    fwprintf_s(pFile, L"\n\n");

    fwprintf_s(pFile, L"[ATLAS_PATH]\n");
    fwprintf_s(pFile, m_pAtlas->GetRelativePath().c_str());
    fwprintf_s(pFile, L"\n\n");


    // 프레임 개수 저장
    wchar_t szNum[50] = {};

    fwprintf_s(pFile, L"[FRAME_COUNT]\n");

    const size_t iFrmCount = m_vecFrm.size();
    _itow_s(static_cast<int>(iFrmCount), szNum, 50, 10);

    fwprintf_s(pFile, szNum);
    fwprintf_s(pFile, L"\n\n");

    for (size_t i = 0; i < iFrmCount; ++i)
    {
        fwprintf_s(pFile, L"[%zd_FRAME]\n", i);

        // LEFT_TOP
        fwprintf_s(pFile, L"[LEFT_TOP]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vLeftTop.x, m_vecFrm[i].vLeftTop.y);

        // SIZE
        fwprintf_s(pFile, L"[SIZE]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vSize.x, m_vecFrm[i].vSize.y);

        // OFFSET
        fwprintf_s(pFile, L"[OFFSET]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vOffset.x, m_vecFrm[i].vOffset.y);

        // DURATION
        fwprintf_s(pFile, L"[DURATION]\n");
        fwprintf_s(pFile, L"%.2f\n", m_vecFrm[i].fDuration);

        fwprintf_s(pFile, L"\n");
    }

    fclose(pFile);
}

void CAnimation::Load(const wstring& _strFileName)
{
    wstring strFilepath = CPathMgr::GetInst()->GetContentPath();
    strFilepath         += L"animation\\";
    strFilepath         += _strFileName;

    // 파일 열기
    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strFilepath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
        MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
        return;
    }

    size_t  iFrameCount = 0;
    wstring strAtlasKey;
    wstring strAltasRelativePath;

    while (true)
    {
        wchar_t szBuffer[256] = {};
        fwscanf_s(pFile, L"%s", szBuffer, 256);

        if (!wcscmp(szBuffer, L"[ANIMATION_NAME]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            SetName(szBuffer);
        }
        else if (!wcscmp(szBuffer, L"[ATLAS_KEY]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            strAtlasKey = szBuffer;
        }
        else if (!wcscmp(szBuffer, L"[ATLAS_PATH]"))
        {
            fwscanf_s(pFile, L"%s", szBuffer, 256);
            strAltasRelativePath = szBuffer;
        }
        else if (!wcscmp(szBuffer, L"[FRAME_COUNT]"))
        {
            fwscanf_s(pFile, L"%zd", &iFrameCount);

            for (size_t i = 0; i < iFrameCount; ++i)
            {
                tAnimFrm frm = {};

                while (true)
                {
                    wchar_t szBuffer[256] = {};
                    fwscanf_s(pFile, L"%s", szBuffer, 256);

                    if (!wcscmp(szBuffer, L"[LEFT_TOP]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &frm.vLeftTop.x, &frm.vLeftTop.y);
                    }

                    else if (!wcscmp(szBuffer, L"[SIZE]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &frm.vSize.x, &frm.vSize.y);
                    }

                    else if (!wcscmp(szBuffer, L"[OFFSET]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &frm.vOffset.x, &frm.vOffset.y);
                    }

                    else if (!wcscmp(szBuffer, L"[DURATION]"))
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

    m_pAtlas = CResMgr::GetInst()->LoadTexture(strAtlasKey, strAltasRelativePath);

    fclose(pFile);
}
