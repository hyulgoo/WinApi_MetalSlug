#include "pch.h"
#include "Game\04.Object\Character\Monster\CMonster.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\07.MonsterState\CState.h"

#include "Game\07.MonsterState\Idle\CIdle.h"
#include "Game\07.MonsterState\Attack\CAttack.h"
#include "Game\07.MonsterState\Trace\CTrace.h"
#include "Game\07.MonsterState\Dead\CDead.h"
#include "Game\07.MonsterState\Trace\CTraceSly.h"
#include "Game\07.MonsterState\Attack\CAttackCQC.h"

#include "Game\02.Manager\08.Camera\CCamera.h"


CMonster::CMonster()
    : m_iHP(10)
    , m_pTarget(nullptr)
    , m_fTime(0.f)
    , m_iMaxCount(7)
    , m_fRatio(0.f)
    , m_bAttack(false)
{
    CreateAnimator();
    CreateCollider();
    CreateAI();

    GetAI()->AddState(L"Idle", new CIdle);
    GetAI()->AddState(L"Trace", new CTrace);
    GetAI()->AddState(L"Attack", new CAttack);
    GetAI()->AddState(L"Dead", new CDead);
    GetAI()->AddState(L"TraceSly", new CTraceSly);
    GetAI()->AddState(L"AttackCQC", new CAttackCQC);
    GetAI()->ChangeState(L"Idle");

    SetMstInfo(400.f, 3.f, 700.f, 10, 80.f, 0.6f);

    m_irand = rand() % 10;
}

CMonster::~CMonster()
{
}

void CMonster::tick()
{
    CObj::tick();

    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

    if (GetPos().x < -30 && !IsDead())
        SetDead();
}

void CMonster::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CMonster::SetMstInfo(const float _fAttackRange, const float _fAttackDelay, const float _DetectRange, const UINT _HP, const float _fSpeed, const float _fDeadTime)
{
    m_tInfo.m_fAttackRange = _fAttackRange;
    m_tInfo.m_fAttackDelay = _fAttackDelay;
    m_tInfo.m_fDetectRange = _DetectRange;
    m_tInfo.m_iHP          = _HP;
    m_tInfo.m_fSpeed       = _fSpeed;
    m_tInfo.m_fDeadTime    = _fDeadTime;
}
