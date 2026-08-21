#include "pch.h"
#include "CZombie_oldman.h"

#include "CAI.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CZB_Bullet.h"
#include "CCollider.h"
#include "CDeadEffect.h"
CZombie_oldman::CZombie_oldman()
{
	init();
}

CZombie_oldman::CZombie_oldman(const CZombie_oldman& _other)
{
	init();
}

CZombie_oldman::~CZombie_oldman()
{
}

void CZombie_oldman::init()
{
	SetPos(Vec2(1000.f, 300.f));
	SetScale(Vec2(90.f, 130.f));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
	CreatePixelCollider();
	CreateRigidbody();
	CreateAnimator2();

	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
	LoadAnimation();


	SetMstHP(10);
	SetMstAtkDelay(2.04f);
	SetMstAtkRange(300.f);
	SetMstSpeed(80.f);
}

void CZombie_oldman::tick()
{
	CMonster::tick();

	m_pCurState = GetAI()->GetState();

	if (m_pPrevState != m_pCurState)
	{
		m_pPrevState = m_pCurState;
		Play(m_bDir);
	}

	m_pCurState = GetAI()->GetState();

	if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		m_fAttackTime += DT;
		if (m_fAttackTime > 1.08f)
		{
			float Offset = 0.f;
			CZB_Bullet* ZBAT = new CZB_Bullet;
			if (m_bDir)
			{
				ZBAT->SetDir(false);
				Offset = 30.f;
			}
			else
			{
				ZBAT->SetDir(true);
				Offset = -30.f;
			}
			if (m_bDir)
				ZBAT->AddVelocityRight();
			else
				ZBAT->AddVelocityLeft();
			Instantiate(ZBAT, Vec2(GetPos().x + Offset, GetPos().y - 70), LAYER::MONSTER_PROJECTILE);
			m_fAttackTime = 0.f;
			
		}
	}
	else
		m_fAttackTime = 0.f;
}

void CZombie_oldman::render(const HDC _dc)
{

	CMonster::render(_dc);
}

void CZombie_oldman::BeginOverlap(CCollider* _pOther)
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

void CZombie_oldman::OnOverlap(CCollider* _pOther)
{
}

void CZombie_oldman::LoadAnimation()
{
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_NOTHING.anim");
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_OLD_ATTACK.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_OLD_ATTACK_EFFECT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_OLD_IDLE.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_OLD_RUN.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CZombie_oldman::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CZombie_oldman::PlayEffect(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator2()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator2()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CZombie_oldman::Play(const bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;
	if (GetAI()->FindState(L"Idle") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_OLD_IDLE.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Trace") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_OLD_RUN.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_OLD_ATTACK_EFFECT.anim", false, bLeft);
		PlayEffect(L"ZB_OLD_ATTACK.anim", false, bLeft);
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