#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CMonolith.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"
#include "Game\04.Object\Bullet\Enemy\CCrushArea.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

CMonolith::CMonolith()
{
    const CDataSection& tData  = CDataMgr::GetInst()->GetSection(L"boss", L"monolith");
    m_tData.fFallSpeed         = tData.GetFloat(L"fall_speed");
    m_tData.fCrushTime         = tData.GetFloat(L"crush_time");
    m_tData.fShakeRange        = tData.GetFloat(L"shake_range");
    m_tData.fShakeSpeed        = tData.GetFloat(L"shake_speed");
    m_tData.fShakeTime         = tData.GetFloat(L"shake_time");
    m_tData.fHoldTime          = tData.GetFloat(L"hold_time");
    m_tData.fSinkFrameTime     = tData.GetFloat(L"sink_frame_time");
    m_tData.fSinkBullet        = tData.GetFloat(L"sink_bullet");
    m_tData.fSinkGrenade       = tData.GetFloat(L"sink_grenade");
    m_tData.fCollapseFrameTime = tData.GetFloat(L"collapse_frame_time");
    m_tData.fTopInset          = tData.GetFloat(L"top_inset");

    CreateAnimator();
    CreateCollider();
    GetAnimator()->LoadAnimation(L"Boss\\MONOLITH.anim");
}

CMonolith::~CMonolith()
{
}

void CMonolith::tick()
{
    // 첫 tick: 만든 자리(조준 빔이 닿은 땅)를 기억하고, 아래 끝이 화면 위 끝에 오게 올라가서 떨어지기 시작한다
    if (!m_bStart)
    {
        m_bStart   = true;
        m_fGroundY = GetPos().y;
        SetPos(Vec2(GetPos().x, min(m_fGroundY, CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)).y)));
        GetAnimator()->Play(L"MONOLITH.anim", false);
    }

    if (!m_bLanded)
    {
        const float fY = GetPos().y + m_tData.fFallSpeed * DT;
        SetPos(Vec2(GetPos().x, min(fY, m_fGroundY)));
        if (fY >= m_fGroundY)
            Land();
    }
    else
    {
        // 저절로 가라앉는다: 박히고 hold_time이 지나면 sink_frame_time마다 한 장.
        // 보스가 부서진 뒤에는 기다리지 않고 collapse_frame_time마다 한 장 (한 장의 시간이 0이면 바로 다 가라앉는다)
        const auto  Frames    = [this](const float _fTime, const float _fFrameTime)
        { return _fFrameTime > 0.f ? _fTime / _fFrameTime : static_cast<float>(FrmCount()); };
        const float fPrevTime = m_fTime;
        m_fTime               += DT;
        ++m_iLandTick;
        if (m_bCollapse)
            Sink(Frames(DT, m_tData.fCollapseFrameTime));
        else if (m_fTime > m_tData.fHoldTime)
            Sink(Frames(m_fTime - max(fPrevTime, m_tData.fHoldTime), m_tData.fSinkFrameTime));
    }

    // 다 가라앉았다: 돌무더기를 잠깐 남기고 사라진다
    const CAnimation* pAnim = GetAnimator()->GetAnimation();
    if (m_bLanded && FrmCount() > 0 && SunkFrames() >= FrmCount())
    {
        if (!IsDead())
        {
            Instantiate(new CAnimEffect(L"Boss\\MONOLITH_RUBBLE.anim", true), GetPos(), LAYER::DEFAULT);
            SetDead();
        }
        return;
    }

    CObj::tick();

    // 가라앉는 그림은 재생으로 넘기지 않고 가라앉은 정도로 고른다
    if (nullptr != pAnim)
        GetAnimator()->GetAnimation()->SetFrmNum(SunkFrames());
    UpdateCollider();
}

void CMonolith::Land()
{
    m_bLanded = true;

    // 보스가 부서진 뒤에 떨어진 비석은 깔지 않는다
    if (m_bCollapse)
        return;

    CCamera::GetInst()->CameraShake(m_tData.fShakeRange, m_tData.fShakeSpeed, m_tData.fShakeTime);

    // 박히는 순간 비석 자리에 있으면 깔린다 (잠깐만 있는 판정)
    const tAnimFrm frm   = CurFrm();
    const Vec2     vSize = frm.vSize * ANIM_MAGNI;
    const Vec2     vPos  = GetPos() + Vec2(frm.vOffset.x * ANIM_MAGNI + vSize.x / 2.f, frm.vOffset.y * ANIM_MAGNI + vSize.y / 2.f);
    Instantiate(new CCrushArea(vSize, m_tData.fCrushTime), vPos, LAYER::MONSTER_PROJECTILE);
}

void CMonolith::Collapse()
{
    // 기다리지 않고 바로 빨리 가라앉는다 (tick)
    m_bCollapse = true;
}

void CMonolith::Sink(const float _fFrames)
{
    m_fSunk = min(m_fSunk + _fFrames, static_cast<float>(FrmCount()));
}

int CMonolith::FrmCount() const
{
    const CAnimation* pAnim = GetAnimator()->GetAnimation();
    return nullptr == pAnim ? 0 : pAnim->GetFrmCount();
}

int CMonolith::SunkFrames() const
{
    return static_cast<int>(m_fSunk);
}

tAnimFrm CMonolith::CurFrm() const
{
    // 다 가라앉은 뒤(사라지기 직전)에는 마지막 그림
    const CAnimation* pAnim = GetAnimator()->GetAnimation();
    if (nullptr == pAnim)
        return tAnimFrm{};
    return pAnim->GetFrm(SunkFrames());
}

void CMonolith::UpdateCollider()
{
    // 맞는 범위는 지금 보이는 비석 그림 그대로 (위치는 아래 끝이라 위로 올린다)
    const tAnimFrm frm   = CurFrm();
    const Vec2     vSize = frm.vSize * ANIM_MAGNI;
    GetCollider()->SetScale(vSize);
    GetCollider()->SetOffsetPos(Vec2(frm.vOffset.x * ANIM_MAGNI + vSize.x / 2.f, frm.vOffset.y * ANIM_MAGNI + vSize.y / 2.f));
}

bool CMonolith::GetObstacleBox(Vec2& _vLeftTop, Vec2& _vRightBottom) const
{
    // 박히고 두 tick 뒤부터 밟고 막는다. 깔림 판정(CCrushArea)은 박힌 다음 프레임에 생겨 그 프레임 충돌 검사에서 깔리는데,
    // 그 전에 막으면 비석 자리에 있던 플레이어를 옆으로 밀어내 깔리지 않는다 (박힌 tick, 다음 tick 모두 플레이어가 비석보다 늦게 움직인다)
    if (!m_bLanded || m_iLandTick < 2)
        return false;

    // 상자는 지금 보이는 그림 (위 끝은 밟는 높이라 top_inset만큼 내린다, 아래 끝은 박힌 땅)

    const tAnimFrm frm = CurFrm();
    _vLeftTop          = GetPos() + frm.vOffset * ANIM_MAGNI + Vec2(0.f, m_tData.fTopInset);
    _vRightBottom      = Vec2(_vLeftTop.x + frm.vSize.x * ANIM_MAGNI, GetPos().y);
    return true;
}

void CMonolith::BeginOverlap(CCollider* _pOther)
{
    // 박힌 비석은 맞을 때마다 바로 더 가라앉는다 (떨어지는 동안은 총알만 막는다)
    if (!m_bLanded)
        return;

    switch (_pOther->GetOwner()->GetLayer())
    {
    case LAYER::PLAYER_PROJECTILE:      Sink(m_tData.fSinkBullet);  break;
    case LAYER::PLAYER_FIREPROJECTILE:  Sink(m_tData.fSinkGrenade); break;
    default:                                                        break;
    }
}
