#include "pch.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"

#include "Game\04.Object\CObj.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
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

CAnimation::~CAnimation()
{
}

void CAnimation::tick(const bool _bRepeat)
{
    const vector<tAnimFrm>& vecFrm = Frms();
    if (m_iCurFrm >= static_cast<int>(vecFrm.size()))
        m_iCurFrm = 0;

    m_iTickStartFrm = m_iCurFrm;
    m_iTickLoops    = 0;

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
    if (vecFrm.empty() || nullptr == m_pAtlas || m_iCurFrm >= static_cast<int>(vecFrm.size()))
        return;

    // 월드 오브젝트는 카메라만큼 옮긴 화면 위치에 그린다
    const Vec2 vPos = m_pAnimator->GetOwner()->GetScreenPos();

    const tAnimFrm frm = vecFrm[m_iCurFrm];

    // 상체·하체로 나눠 그리는 애니메이션은 몸 기준점에 맞춰 조금 옮긴다 (그리기만 하고 상태는 바꾸지 않는다)
    Vec2 m_vOffset = {};
    if (m_tPrintMode == SETPRINT::UPPER)
        m_vOffset = Vec2(-5.7f, -25.f);
    else if (m_tPrintMode == SETPRINT::LOWER)
        m_vOffset = Vec2(-5.7f, 0.f);

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

void CAnimation::SetAllFrmInfo(const vector<tAnimFrm>& _Other)
{
    m_pData  = nullptr;
    m_vecFrm = _Other;
    if (m_iCurFrm >= static_cast<int>(m_vecFrm.size()))
        m_iCurFrm = m_vecFrm.empty() ? 0 : static_cast<int>(m_vecFrm.size()) - 1;
}
