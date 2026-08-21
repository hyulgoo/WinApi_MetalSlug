#include "pch.h"
#include "CMonster.h"
#include "CPlayer.h"

#include "CTimeMgr.h"
#include "CResMgr.h"

#include "CCollider.h"
#include "CRigidbody.h"
#include "CAI.h"
#include "CState.h"

#include "CIdle.h"
#include "CAttack.h"
#include "CTrace.h"
#include "CDead.h"
#include "CTraceSly.h"
#include "CAttackCQC.h"

#include "CCamera.h"
#include "CEngine.h"


CMonster::CMonster()
	: m_pTarget(nullptr)
	, m_fTime(0.f)
	, m_iMaxCount(7)
	, m_fRatio(0.f)
	, m_bAttack(false)
	, m_iHP(10)
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

void CMonster::render(HDC _dc)
{
	CObj::render(_dc);
}

void CMonster::SetMstInfo(float _fAttackRange, float _fAttackDelay, float _DetectRange, UINT HP, float _fSpeed, float _fDeadTime)
{
	m_tInfo.m_fAttackRange = _fAttackRange;
	m_tInfo.m_fAttackDelay = _fAttackDelay;
	m_tInfo.m_fDetectRange = _DetectRange;
	m_tInfo.m_iHP = HP;
	m_tInfo.m_fSpeed = _fSpeed;
	m_tInfo.m_fDeadTime = _fDeadTime;
}