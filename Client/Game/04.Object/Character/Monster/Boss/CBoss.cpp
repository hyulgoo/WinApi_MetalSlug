#include "pch.h"
#include "Game\04.Object\Character\Monster\Boss\CBoss.h"

#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"
#include "Game\04.Object\Bullet\Enemy\CMonolith.h"
#include "Game\04.Object\Bullet\Enemy\CMonolithBeam.h"
#include "Game\04.Object\Character\Monster\Boss\CBossWreck.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include <iterator>

namespace
{
    // 애니메이션 이름 (파일은 animation\Boss\ 에 같은 이름으로 있다)
    constexpr const wchar_t* IDLE    = L"BOSS_IDLE.anim";      // 몸 전체: 초록 돔, 받침대, 돌무더기
    constexpr const wchar_t* FIRE    = L"BOSS_ATTACK.anim";    // 돔이 빛기둥을 쏜다 (몸 위에 돔 자리를 맞춰 겹쳐 그린다)
    constexpr const wchar_t* NOTHING = L"BOSS_NOTHING.anim";   // 겹쳐 그릴 것이 없을 때

    // 먼지 기둥이 생기는 가로 자리 ([appear] burst_spread에 곱한다). 난수 대신 정해 둔 차례로
    constexpr float BURST_X[] = { 0.f, -1.f, 1.f, -0.5f, 0.5f };
}

CBoss::CBoss()
{
    const CDataSection& tBoss = CDataMgr::GetInst()->GetSection(L"boss", L"boss");
    SetScale(tBoss.GetVec2(L"size"));
    SetHP(tBoss.GetInt(L"hp"));
    GetCollider()->SetScale(tBoss.GetVec2(L"collider_size"));
    GetCollider()->SetOffsetPos(tBoss.GetVec2(L"collider_offset"));
    m_tData.fWaitTime    = tBoss.GetFloat(L"wait_time");
    m_tData.iSummonFrame = tBoss.GetInt(L"summon_frame");

    const CDataSection& tAppear = CDataMgr::GetInst()->GetSection(L"boss", L"appear");
    m_tData.fAppearTime         = tAppear.GetFloat(L"time");
    m_tData.fAppearDepth        = tAppear.GetFloat(L"depth");
    m_tData.fShakeRange         = tAppear.GetFloat(L"shake_range");
    m_tData.fShakeSpeed         = tAppear.GetFloat(L"shake_speed");
    m_tData.fBurstInterval      = tAppear.GetFloat(L"burst_interval");
    m_tData.fBurstSpread        = tAppear.GetFloat(L"burst_spread");

    // 돔에 새긴 글자가 뒤집히지 않게 플레이어 쪽으로 돌아보지 않는다
    SetFixedFacing();

    // 몸은 Animator2, 빛기둥을 쏘는 돔은 Animator (Animator2를 먼저, Animator를 위에 그린다)
    CreateAnimator2();
    GetAnimator2()->LoadAnimation(L"Boss\\BOSS_IDLE.anim");
    GetAnimator()->LoadAnimation(L"Boss\\BOSS_ATTACK.anim");
    GetAnimator()->LoadAnimation(L"Boss\\BOSS_NOTHING.anim");

    // 상태는 첫 tick에 자리를 정한 뒤 등장(APPEAR)부터 시작한다
    GetAI()->AddState(MON_STATE::APPEAR);
    GetAI()->AddState(MON_STATE::WAIT);
    GetAI()->AddState(MON_STATE::ATTACK);
    GetAI()->AddState(MON_STATE::DEAD);
}

CBoss::~CBoss()
{
}

void CBoss::tick()
{
    // 첫 tick: 레벨 파일의 자리(위치는 만든 뒤에 정해진다)를 기억하고 땅 아래로 내려가 솟아오르기 시작한다.
    // 몸은 다 올라왔을 때의 발밑에서 잘려 땅 위로 나온 만큼만 보인다
    if (!m_bPlaced)
    {
        m_bPlaced = true;
        m_vHome   = GetPos();
        SetPos(Vec2(m_vHome.x, m_vHome.y + m_tData.fAppearDepth));
        if (CAnimation* pIdle = GetAnimator2()->FindAnimation(IDLE))
            pIdle->SetClipBottom(GetFeetY());
        CCamera::GetInst()->CameraShake(m_tData.fShakeRange, m_tData.fShakeSpeed, m_tData.fAppearTime);
        GetAI()->ChangeState(MON_STATE::APPEAR);
    }

    CMonster::tick();
}

void CBoss::BeginOverlap(CCollider* _pOther)
{
    // 솟아오르는 동안은 피해를 받지 않는다 (총알은 막는다)
    const MON_STATE eState = GetAI()->GetState();
    if (MON_STATE::APPEAR == eState || MON_STATE::END == eState)
        return;

    CMonster::BeginOverlap(_pOther);
}

void CBoss::PlayStateAnim(const MON_STATE _eState)
{
    switch (_eState)
    {
    case MON_STATE::APPEAR:
    case MON_STATE::WAIT:
        PlayIfNot(GetAnimator2(), IDLE, true);
        PlayAnim(GetAnimator(), NOTHING, false, true);
        break;
    case MON_STATE::ATTACK:
        PlayIfNot(GetAnimator2(), IDLE, true);
        PlayAnim(GetAnimator(), FIRE, false, true);
        break;
    default:
        break;
    }
}

void CBoss::PlayIfNot(CAnimator* _pAnimator, const wchar_t* _szName, const bool _bRepeat)
{
    // 몸은 상태가 바뀌어도 돔이 일렁이는 것을 처음부터 다시 틀지 않는다
    CAnimation* pCur = _pAnimator->GetAnimation();
    if (nullptr != pCur && pCur->GetName() == _szName)
        return;
    PlayAnim(_pAnimator, _szName, _bRepeat, true);
}

const CAnimation* CBoss::GetAttackAnim() const
{
    return GetAnimator()->GetAnimation();
}

int CBoss::GetAttackFrame(const MON_STATE /*_eState*/) const
{
    return m_tData.iSummonFrame;
}

void CBoss::OnAttackFrame(const MON_STATE /*_eState*/)
{
    // 하늘에서 조준 빔이 내려와 플레이어를 따라온다. 빔이 멈춘 자리에 비석이 떨어진다
    const CPlayer* pPlayer = FindPlayer();
    const float    fX      = nullptr != pPlayer ? pPlayer->GetPos().x : GetPos().x;
    Instantiate(new CMonolithBeam, Vec2(fX, GetFeetY()), LAYER::EFFECT);
}

void CBoss::NextAttackCycle()
{
    // 쫓아가지 않는다. 다음 공격까지 기다린다
    GetAI()->ChangeState(MON_STATE::WAIT);
}

float CBoss::GetStateDuration(const MON_STATE _eState) const
{
    switch (_eState)
    {
    case MON_STATE::APPEAR: return m_tData.fAppearTime;
    case MON_STATE::WAIT:   return m_tData.fWaitTime;
    default:                return 0.f;
    }
}

void CBoss::Emerge(const float _fRatio)
{
    // 처음에는 빠르게 올라오다 끝에서 천천히 멈춘다: 남은 깊이 = depth x (1 - t)^2
    const float fRemain = 1.f - _fRatio;
    SetPos(Vec2(m_vHome.x, m_vHome.y + m_tData.fAppearDepth * fRemain * fRemain));

    // 올라오는 동안 발밑에서 먼지 기둥이 솟는다 (돌무덤이 부서진다)
    const float fTime = _fRatio * m_tData.fAppearTime;
    while (m_tData.fBurstInterval > 0.f && m_fNextBurst <= fTime && m_fNextBurst < m_tData.fAppearTime)
    {
        const float fX = m_vHome.x + BURST_X[static_cast<size_t>(m_iBurst) % std::size(BURST_X)] * m_tData.fBurstSpread;
        Instantiate(new CAnimEffect(L"Boss\\BOSS_EMERGE.anim", true), Vec2(fX, GetFeetY()), LAYER::EFFECT);
        ++m_iBurst;
        m_fNextBurst += m_tData.fBurstInterval;
    }

    // 다 올라왔다: 더 자르지 않는다
    if (_fRatio >= 1.f)
    {
        SetPos(m_vHome);
        if (CAnimation* pIdle = GetAnimator2()->FindAnimation(IDLE))
            pIdle->ClearClip();
    }
}

void CBoss::SpawnDeathEffect()
{
    // 돔이 부서진 잔해. 그 위로 폭발이 이어지고, 끝나면 플레이어가 승리 포즈를 한다
    Instantiate(new CBossWreck, GetPos(), LAYER::DEFAULT);

    // 남은 조준 빔은 비석을 떨어뜨리지 않고 사라지고, 남은 비석은 빨리 가라앉는다 (더는 깔리지 않는다)
    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
    if (nullptr == pLevel)
        return;
    for (CObj* pObj : pLevel->GetLayer(LAYER::EFFECT))
    {
        if (nullptr != dynamic_cast<CMonolithBeam*>(pObj) && !pObj->IsDead())
            pObj->SetDead();
    }
    for (CObj* pObj : pLevel->GetLayer(LAYER::OBSTACLE))
    {
        if (CMonolith* pMonolith = dynamic_cast<CMonolith*>(pObj))
            pMonolith->Collapse();
    }
}
