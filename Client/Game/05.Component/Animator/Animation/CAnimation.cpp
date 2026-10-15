#include "pch.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"

#include "Game\04.Object\CObj.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\06.Resource\Anim\CAnimData.h"
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
    // 모든 애니메이션이 함께 쓰는 확대용 버퍼 (처음 한 번만 만든다). 그릴 때마다 쓰는 영역을 덮어쓰므로 채워 둘 필요가 없다
    // (예전에는 애니메이션을 만들 때마다 화면 크기 버퍼 전체를 다시 칠했고, SetDCBrushColor는 브러시를 선택하지 않아 효과도 없었다)
    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    m_pBackBuffer          = CResMgr::GetInst()->CreateTexture(L"AnimBackBuffer", static_cast<int>(vResolution.x), static_cast<int>(vResolution.y));
}

void CAnimation::SetData(const CAnimData* _pData)
{
    m_pData = _pData;
    m_vecFrm.clear();
    if (nullptr == _pData)
        return;
    SetName(_pData->GetName());
    m_pAtlas = _pData->GetAtlas();
}

const vector<tAnimFrm>& CAnimation::Frms() const
{
    return nullptr != m_pData ? m_pData->GetFrames() : m_vecFrm;
}

void CAnimation::MakeOwnFrames()
{
    if (nullptr == m_pData)
        return;
    m_vecFrm = m_pData->GetFrames();
    m_pData  = nullptr;
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

void CAnimation::tick(const bool _bRepeat)
{
    m_iTickStartFrm = m_iCurFrm;
    m_iTickLoops    = 0;

    const vector<tAnimFrm>& vecFrm = Frms();
    if (m_bFinish || vecFrm.empty()) // 이미 재생이 끝났거나 프레임이 없으면 return
        return;

    // 예전에는 tick마다 많아야 한 프레임만 넘기고 남은 시간을 버려, 재생 속도가 FPS에 따라 달랐다
    // (60 FPS에서 0.12초 프레임이 0.133초, 30 FPS에서 0.167초). 남은 시간을 다음 프레임으로 넘긴다
    m_fAccTime += DT;
    while (vecFrm[m_iCurFrm].fDuration < m_fAccTime)
    {
        m_fAccTime -= vecFrm[m_iCurFrm].fDuration;

        if (m_iCurFrm + 1 < static_cast<int>(vecFrm.size()))
        {
            ++m_iCurFrm;
            continue;
        }

        // 마지막 프레임까지 다 보여 줬다. 반복이면 처음으로(재생 시간이 0뿐인 애니메이션은 끝낸다)
        if (_bRepeat && m_iTickLoops < 1000)
        {
            m_iCurFrm = 0;
            ++m_iTickLoops;
            continue;
        }

        m_bFinish  = true;
        m_fAccTime = 0.f;
        break;
    }
}

bool CAnimation::IsFrameEntered(const int _iFrm) const
{
    if (0 == m_iTickLoops)
        return m_iTickStartFrm < _iFrm && _iFrm <= m_iCurFrm;
    if (1 == m_iTickLoops)
        return m_iTickStartFrm < _iFrm || _iFrm <= m_iCurFrm;
    return true;
}

void CAnimation::render(const HDC _dc)
{
    const vector<tAnimFrm>& vecFrm = Frms();
    if (vecFrm.empty() || nullptr == m_pAtlas)
        return;

    // 월드 오브젝트는 카메라만큼 옮긴 화면 위치에 그린다
    const Vec2 vPos = m_pAnimator->GetOwner()->GetScreenPos();

    const tAnimFrm frm = vecFrm[m_iCurFrm];
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
    if (m_iCurFrm + 1 >= GetFrmCount())
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
    MakeOwnFrames();
    if (m_vecFrm.empty())
        return;
    m_vecFrm[m_iCurFrm] = _Other;
}

void CAnimation::SetAllFrmInfo(const vector<tAnimFrm>& _Other)
{
    m_pData  = nullptr;
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
        ReportError(L"파일 저장 실패", szStr);
        return;
    }

    // Animation 이름 저장
    fwprintf_s(pFile, L"\n");

    fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
    fwprintf_s(pFile, L"%s", GetName().c_str());
    fwprintf_s(pFile, L"\n\n");

    // 아틀라스 텍스처 키값 저장
    fwprintf_s(pFile, L"[ATLAS_KEY]\n");
    fwprintf_s(pFile, L"%s", m_pAtlas->GetKey().c_str());
    fwprintf_s(pFile, L"\n\n");

    fwprintf_s(pFile, L"[ATLAS_PATH]\n");
    fwprintf_s(pFile, L"%s", m_pAtlas->GetRelativePath().c_str());
    fwprintf_s(pFile, L"\n\n");


    // 프레임 개수 저장
    wchar_t szNum[50] = {};

    fwprintf_s(pFile, L"[FRAME_COUNT]\n");

    const vector<tAnimFrm>& vecFrm    = Frms();
    const size_t            iFrmCount = vecFrm.size();
    _itow_s(static_cast<int>(iFrmCount), szNum, 50, 10);

    fwprintf_s(pFile, L"%s", szNum);
    fwprintf_s(pFile, L"\n\n");

    for (size_t i = 0; i < iFrmCount; ++i)
    {
        fwprintf_s(pFile, L"[%zd_FRAME]\n", i);

        // LEFT_TOP
        fwprintf_s(pFile, L"[LEFT_TOP]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", vecFrm[i].vLeftTop.x, vecFrm[i].vLeftTop.y);

        // SIZE
        fwprintf_s(pFile, L"[SIZE]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", vecFrm[i].vSize.x, vecFrm[i].vSize.y);

        // OFFSET
        fwprintf_s(pFile, L"[OFFSET]\n");
        fwprintf_s(pFile, L"%.1f %.1f\n", vecFrm[i].vOffset.x, vecFrm[i].vOffset.y);

        // DURATION
        fwprintf_s(pFile, L"[DURATION]\n");
        fwprintf_s(pFile, L"%.2f\n", vecFrm[i].fDuration);

        fwprintf_s(pFile, L"\n");
    }

    fclose(pFile);
}

