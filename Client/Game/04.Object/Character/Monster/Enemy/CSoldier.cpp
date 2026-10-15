#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CSoldier.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\04.Object\Bullet\Enemy\CSLDCQC.h"
#include "Game\04.Object\Bullet\Enemy\CSLD_Grenade.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CSoldier::CSoldier()
	: m_iGrenadeFrame(0)
	, m_iKnifeFrame(0)
	, m_fCQCRatio(0.f)
	, m_fSlyRatio(0.f)
{
	// 공격 프레임은 예전 타이머(수류탄 0.8초, 칼 0.7초)와 같거나 가장 가까운 프레임의 시작이다 (monster.ini)
	const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"monster", L"soldier");
	SetScale(tData.GetVec2(L"size"));
	LoadStats(tData);
	SetSlySpeed(GetSpeed() / tData.GetFloat(L"sly_speed_div"));
	m_iGrenadeFrame  = tData.GetInt(L"grenade_frame");
	m_iKnifeFrame    = tData.GetInt(L"knife_frame");
	m_fCQCRatio      = tData.GetFloat(L"cqc_ratio");
	m_fSlyRatio      = tData.GetFloat(L"sly_ratio");
	m_vGrenadeOffset = tData.GetVec2(L"grenade_offset");

	CreatePixelCollider();
	CreateRigidbody();

	// 사망 애니메이션은 사망 이펙트(CDeadEffect)가 따로 불러 쓴다
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_IDLE.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_RUN_LEFT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_WALK_LEFT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_BOMB.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_CQC.anim");

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
	return MON_STATE::ATTACK_CQC == _eState ? m_iKnifeFrame : m_iGrenadeFrame;
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
	const Vec2 vOffset = Vec2(GetDir() ? m_vGrenadeOffset.x : -m_vGrenadeOffset.x, m_vGrenadeOffset.y);
	Instantiate(pGrenade, GetPos() + vOffset, LAYER::MONSTER_PROJECTILE);
}

MON_STATE CSoldier::ChooseAttack(const float _fDist) const
{
	if (_fDist < GetAttackRange() * m_fCQCRatio)
		return MON_STATE::ATTACK_CQC;
	if (_fDist < GetAttackRange() * m_fSlyRatio)
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
	Instantiate(pEffect, Vec2(GetPos().x, GetPos().y + GetDeathOffsetY()), LAYER::DEFAULT);
}
