#include "pch.h"
#include "CSLD_Grenade.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CRigidbody.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CPixelCollider.h"
#include "CSLDBombEffect.h"


CSLD_Grenade::CSLD_Grenade()
	: m_bDir(false)
	, m_fDeadTime(0.f)
	, m_bStart(true)
	, m_bBounce(false)
	, m_fShotTime()
	, m_vPower(Vec2(400.f, -600.f))

{
	SetScale(Vec2(20.f, 20.f));
	CreateAnimator();
	CreateCollider();
	CreatePixelCollider();
	CreateRigidbody();

	GetRigidbody()->SetGravity(true);
	GetRigidbody()->SetGravityAccel(1500.f);
	GetRigidbody()->SetFrictionScale(3.f);
	GetRigidbody()->SetFriction(300.f);
	GetPixelCollider()->SetGravity();
	GetCollider()->SetScale(Vec2(10.f, 10.f));
	m_pTexture = CResMgr::GetInst()->LoadTexture(L"Soldier", L"texture\\Soldier.bmp");
	GetAnimator()->LoadAnimaton(L"Enemy//SOLDIER_BOMB_PROJECTILE.anim");
}

CSLD_Grenade::~CSLD_Grenade()
{
}
void CSLD_Grenade::tick()
{
	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

	CObj::tick();

	if (m_bStart)
	{
		GetAnimator()->Play(L"SOLDIER_BOMB_PROJECTILE.anim", true);
		if (m_bDir)
			GetAnimator()->GetAnimation()->SetPlayRight();
		else
			GetAnimator()->GetAnimation()->SetPlayLeft();
		AddVelocity(m_bDir);
		m_bStart = false;
	}

	if (GetRigidbody()->IsGround())
	{
		CSLDBombEffect* pEffect = new CSLDBombEffect;
		pEffect->SetDir(m_bDir);
		pEffect->SetState(true);
		Instantiate(pEffect, GetPos(), LAYER::EFFECT);
		if(!IsDead())
		 SetDead();		
	}
}

void CSLD_Grenade::render(HDC _dc)
{
	CObj::render(_dc);
}

void CSLD_Grenade::BeginOverlap(CCollider* _pOther)
{
	CSLDBombEffect* pEffect = new CSLDBombEffect;
	pEffect->SetDir(m_bDir);
	pEffect->SetState(true);
	Instantiate(pEffect, GetPos(), LAYER::EFFECT);
	SetDead();
}

void CSLD_Grenade::AddVelocity(bool _bDir)
{
	m_bDir = _bDir;
	if (_bDir)
		GetRigidbody()->AddVelocity(Vec2(m_vPower.x, m_vPower.y));
	else
		GetRigidbody()->AddVelocity(Vec2(-m_vPower.x, m_vPower.y));
}