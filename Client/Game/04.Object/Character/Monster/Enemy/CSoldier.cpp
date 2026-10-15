#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CSoldier.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\04.Object\Bullet\Enemy\CSLDCQC.h"
#include "Game\04.Object\Bullet\Enemy\CSLD_Grenade.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

namespace
{
	// SOLDIER_BOMB.anim: 수류탄을 놓는 프레임. 예전 0.8초 타이머 = 10번 프레임 시작(0.08초 x 10)
	constexpr int GRENADE_THROW_FRAME = 10;
	// SOLDIER_CQC.anim: 칼을 휘두르는 짧은 프레임(9·10번, 0.04초). 예전 0.7초 타이머에 가장 가까운 프레임 시작(9번, 0.71초)
	constexpr int KNIFE_HIT_FRAME = 9;

	// 공격 범위에 대한 비율: 이보다 가까우면 칼, 그다음까지는 천천히 다가간다
	constexpr float CQC_RATIO = 0.2f;
	constexpr float SLY_RATIO = 0.5f;
}

CSoldier::CSoldier()
{
	SetScale(Vec2(80.f, 130.f));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));

	CreatePixelCollider();
	CreateRigidbody();

	// 사망 애니메이션은 사망 이펙트(CDeadEffect)가 따로 불러 쓴다
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_IDLE.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_RUN_LEFT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_WALK_LEFT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_BOMB.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_CQC.anim");

	SetStats(1, 160.f, 400.f, 700.f);

	GetAI()->AddState(MON_STATE::IDLE);
	GetAI()->AddState(MON_STATE::TRACE);
	GetAI()->AddState(MON_STATE::TRACE_SLY);
	GetAI()->AddState(MON_STATE::ATTACK);
	GetAI()->AddState(MON_STATE::ATTACK_CQC);
	GetAI()->AddState(MON_STATE::DEAD);
	GetAI()->ChangeState(MON_STATE::IDLE);
}

CSoldier::~CSoldier()
{
}

void CSoldier::PlayStateAnim(const MON_STATE _eState)
{
	// 기다리기·달리기·걷기는 왼쪽을, 수류탄·칼은 오른쪽을 보고 그려졌다
	switch (_eState)
	{
	case MON_STATE::IDLE:       PlayAnim(GetAnimator(), L"SOLDIER_IDLE.anim", true, false);       break;
	case MON_STATE::TRACE:      PlayAnim(GetAnimator(), L"SOLDIER_RUN_LEFT.anim", true, false);   break;
	case MON_STATE::TRACE_SLY:  PlayAnim(GetAnimator(), L"SOLDIER_WALK_LEFT.anim", true, false);  break;
	case MON_STATE::ATTACK:     PlayAnim(GetAnimator(), L"SOLDIER_BOMB.anim", false, true);       break;
	case MON_STATE::ATTACK_CQC: PlayAnim(GetAnimator(), L"SOLDIER_CQC.anim", false, true);        break;
	default:                                                                                      break;
	}
}

const CAnimation* CSoldier::GetAttackAnim() const
{
	return GetAnimator()->GetAnimation();
}

int CSoldier::GetAttackFrame(const MON_STATE _eState) const
{
	return MON_STATE::ATTACK_CQC == _eState ? KNIFE_HIT_FRAME : GRENADE_THROW_FRAME;
}

void CSoldier::OnAttackFrame(const MON_STATE _eState)
{
	if (MON_STATE::ATTACK_CQC == _eState)
	{
		CSLDCQC* pCQC = new CSLDCQC;
		pCQC->SetDir(GetDir());
		Instantiate(pCQC, GetPos(), LAYER::MONSTER_PROJECTILE);
		return;
	}

	CSLD_Grenade* pGrenade = new CSLD_Grenade;
	pGrenade->SetDir(GetDir());
	Instantiate(pGrenade, Vec2(GetPos().x, GetPos().y - 50.f), LAYER::MONSTER_PROJECTILE);
}

MON_STATE CSoldier::ChooseAttack(const float _fDist) const
{
	if (_fDist < GetAttackRange() * CQC_RATIO)
		return MON_STATE::ATTACK_CQC;
	if (_fDist < GetAttackRange() * SLY_RATIO)
		return MON_STATE::TRACE_SLY;
	return MON_STATE::ATTACK;
}

void CSoldier::SpawnDeathEffect()
{
	CDeadEffect* pEffect = new CDeadEffect;
	pEffect->SetType(1);
	if (IsFire())
		pEffect->SetFire();
	pEffect->SetDir(GetDir());
	Instantiate(pEffect, Vec2(GetPos().x, GetPos().y + 70.f), LAYER::DEFAULT);
}
