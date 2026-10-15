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

#include <cfloat>
#include <cmath>

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
    m_tData.fHitBullet         = tData.GetFloat(L"hit_bullet");
    m_tData.fHitGrenade        = tData.GetFloat(L"hit_grenade");
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
        // 보스가 부서진 뒤에는 collapse_frame_time 간격으로 빨리 가라앉는다
        const bool bFast = m_bCollapse && m_tData.fCollapseFrameTime > 0.f;
        m_fTime          += bFast ? DT * m_tData.fSinkFrameTime / m_tData.fCollapseFrameTime : DT;
    }

    // 다 가라앉았다: 돌무더기를 잠깐 남기고 사라진다
    const CAnimation* pAnim = GetAnimator()->GetAnimation();
    if (m_bLanded && nullptr != pAnim && SunkFrames() >= static_cast<float>(pAnim->GetFrmCount()))
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
        GetAnimator()->GetAnimation()->SetFrmNum(static_cast<int>(SunkFrames()));
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
    // 기다리지 않고 바로 빨리 가라앉는다
    m_bCollapse = true;
    m_fTime     = max(m_fTime, m_tData.fHoldTime);
}

float CMonolith::SunkFrames() const
{
    if (!m_bLanded || m_fTime <= m_tData.fHoldTime)
        return 0.f;
    // 그림 한 장의 시간이 0이면 바로 다 가라앉는다
    if (m_tData.fSinkFrameTime <= 0.f)
        return FLT_MAX;
    return floorf((m_fTime - m_tData.fHoldTime) / m_tData.fSinkFrameTime);
}

tAnimFrm CMonolith::CurFrm() const
{
    const CAnimation* pAnim = GetAnimator()->GetAnimation();
    if (nullptr == pAnim)
        return tAnimFrm{};
    return pAnim->GetFrm(static_cast<int>(min(SunkFrames(), static_cast<float>(pAnim->GetFrmCount()))));
}

void CMonolith::UpdateCollider()
{
    // 맞는 범위는 지금 보이는 비석 그림 그대로 (위치는 아래 끝이라 위로 올린다)
    const tAnimFrm frm   = CurFrm();
    const Vec2     vSize = frm.vSize * ANIM_MAGNI;
    GetCollider()->SetScale(vSize);
    GetCollider()->SetOffsetPos(Vec2(frm.vOffset.x * ANIM_MAGNI + vSize.x / 2.f, frm.vOffset.y * ANIM_MAGNI + vSize.y / 2.f));
}

bool CMonolith::GetPlatformTop(const float _fX, float& _fTop) const
{
    // 박힌 뒤에만 밟는다. 밟는 윗면은 지금 보이는 그림의 위 끝
    if (!m_bLanded)
        return false;

    const tAnimFrm frm   = CurFrm();
    const float    fLeft = GetPos().x + frm.vOffset.x * ANIM_MAGNI;
    if (_fX < fLeft || _fX > fLeft + frm.vSize.x * ANIM_MAGNI)
        return false;

    _fTop = GetPos().y + frm.vOffset.y * ANIM_MAGNI + m_tData.fTopInset;
    return true;
}

void CMonolith::BeginOverlap(CCollider* _pOther)
{
    // 박힌 비석은 맞으면 더 빨리 가라앉는다 (떨어지는 동안은 총알만 막는다)
    if (!m_bLanded)
        return;

    switch (_pOther->GetOwner()->GetLayer())
    {
    case LAYER::PLAYER_PROJECTILE:      m_fTime += m_tData.fHitBullet;  break;
    case LAYER::PLAYER_FIREPROJECTILE:  m_fTime += m_tData.fHitGrenade; break;
    default:                                                            break;
    }
}
