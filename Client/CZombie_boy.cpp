#include "pch.h"
#include "CZombie_boy.h"

#include "CAI.h"
#include "CState.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CZB_Bullet.h"
#include "CCollider.h"
#include "CDeadEffect.h"

CZombie_boy::CZombie_boy()
{
	init();
}

CZombie_boy::CZombie_boy(const CZombie_boy& _other)
{
	init();
}

CZombie_boy::~CZombie_boy()
{
}

void CZombie_boy::init()
{
	SetPos(Vec2(1000.f, 300.f));
	SetScale(Vec2(70.f, 130.f));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));

	CreatePixelCollider();
	CreateRigidbody();
	CreateAnimator2();

	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
	LoadAnimation();

	SetMstHP(10);
	SetMstAtkDelay(3.f);
	SetMstAtkRange(300.f);
	SetMstSpeed(80.f);
}

void CZombie_boy::tick()
{	
	CMonster::tick();

	m_pCurState = GetAI()->GetState();

	if (m_pPrevState != m_pCurState)
	{
		m_pPrevState = m_pCurState;
		Play(m_bDir);
	}
	
	if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		m_fAttackTime += DT;
		if (m_fAttackTime > 2.64f)
		{
			float Offset = 0.f;
			CZB_Bullet* ZBAT = new CZB_Bullet;
			if (m_bDir)
			{
				ZBAT->SetDir(false);
				Offset = 50.f;
			}
			else
			{
				ZBAT->SetDir(true);
				Offset = -50.f;
			}
			if (m_bDir)
				ZBAT->AddVelocityRight();
			else
				ZBAT->AddVelocituLeft();

			Instantiate(ZBAT, Vec2(GetPos().x + Offset, GetPos().y - 90), LAYER::MONSTER_PROJECTILE);
			m_fAttackTime = 0.f;
		}
	}
	else
	{
		m_bDir = GetAI()->GetMonDir();
		m_fAttackTime = 0.f;
	}

}

void CZombie_boy::render(HDC _dc)
{
	CMonster::render(_dc);
}

void CZombie_boy::BeginOverlap(CCollider* _pOther)
{
	if (_pOther->GetOwner()->GetLayer() == LAYER::PLAYER_PROJECTILE)
	{
		SetMstHP(GetMstInfo().m_iHP - 1);
	}
	else if (_pOther->GetOwner()->GetLayer() == LAYER::PLAYER_FIREPROJECTILE)
	{
		SetMstHP(GetMstInfo().m_iHP - 10);
		m_bFire = true;
	}
	else if (_pOther->GetOwner()->GetLayer() == LAYER::CQC)
	{
		m_bFire = false;
		SetMstHP(GetMstInfo().m_iHP - 4);
	}
}

void CZombie_boy::OnOverlap(CCollider* _pOther)
{
}

void CZombie_boy::LoadAnimation()
{
	GetAnimator2()->LoadAnimaton(L"Enemy//ZB_NOTHING.anim");
	GetAnimator2()->LoadAnimaton(L"Enemy//ZB_BOY_ATTACK.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_BOY_ATTACK_EFFECT.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_BOY_IDLE.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_BOY_RUN.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CZombie_boy::PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CZombie_boy::PlayEffect(const wstring& _strName, bool _bRepeat, bool _bDirRight)
{
	GetAnimator2()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator2()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CZombie_boy::Play(bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;
	if (GetAI()->FindState(L"Idle") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_BOY_IDLE.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Trace") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_BOY_RUN.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_BOY_ATTACK_EFFECT.anim", true, bLeft);
		PlayEffect(L"ZB_BOY_ATTACK.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Dead") == GetAI()->GetState())
	{
		if (m_bFire)
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(2);
			effect->SetFire();
			effect->SetDir(m_bDir);
			Instantiate(effect, Vec2(GetPos().x, GetPos().y + 70), LAYER::DEFAULT);
			if (!IsDead())
				SetDead();
		}
		else
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(2);
			effect->SetDir(m_bDir);
			Instantiate(effect, Vec2(GetPos().x, GetPos().y + 70), LAYER::DEFAULT);
			if (!IsDead())
				SetDead();
		}
	}

}
