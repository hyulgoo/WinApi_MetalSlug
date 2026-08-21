#include "pch.h"
#include "CZombie_RSH.h"

#include "CAI.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CZB_Bullet.h"
#include "CCollider.h"
#include "CDeadEffect.h"
#include "CItem.h"

CZombie_RSH::CZombie_RSH()
{
	init();
}

CZombie_RSH::CZombie_RSH(const CZombie_RSH& _other)
{
	init();
}

CZombie_RSH::~CZombie_RSH()
{
}

void CZombie_RSH::init()
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
	SetMstAtkDelay(3.36f);
	SetMstAtkRange(300.f);
	SetMstSpeed(80.f);
}

void CZombie_RSH::tick()
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
		if (m_fAttackTime > 1.82f)
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

void CZombie_RSH::render(const HDC _dc)
{
	CMonster::render(_dc);
}

void CZombie_RSH::BeginOverlap(CCollider* _pOther)
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

void CZombie_RSH::OnOverlap(CCollider* _pOther)
{
}

void CZombie_RSH::LoadAnimation()
{
	GetAnimator()->LoadAnimation(L"Enemy//ZB_NOTHING.anim");
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_RCH_ATTACK.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_RCH_ATTACK_EFFECT.anim");
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_RCH_IDLE.anim");
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_RCH_RUN.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CZombie_RSH::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CZombie_RSH::PlayEffect(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator2()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator2()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CZombie_RSH::Play(const bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;

	if (GetAI()->FindState(L"Idle") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_NOTHING.anim", true, bLeft);
		PlayEffect(L"ZB_RCH_IDLE.anim", true, bLeft);
	}
	else if (GetAI()->FindState(L"Trace") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_NOTHING.anim", true, bLeft);
		PlayEffect(L"ZB_RCH_RUN.anim", true, bLeft);
	}
	else if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_RCH_ATTACK_EFFECT.anim", false, bLeft);
		PlayEffect(L"ZB_RCH_ATTACK.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Dead") == GetAI()->GetState())
	{
		if (m_bFire)
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(2);
			effect->SetFire();
			effect->SetDir(m_bDir);
			Instantiate(effect, GetPos(), LAYER::DEFAULT);

			CItem* pItem = new CItem;
			pItem->SetPos(GetPos());
			pItem->SetBB(Getrand());
			Instantiate(pItem, GetPos(), LAYER::ITEM);

			if (!IsDead())
				SetDead();
		}
		else
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(2);
			effect->SetDir(m_bDir);
			Instantiate(effect, Vec2(GetPos().x, GetPos().y + 70), LAYER::DEFAULT);

			CItem* pItem = new CItem;
			pItem->SetPos(GetPos());
			pItem->SetBB(Getrand());
			Instantiate(pItem, Vec2(GetPos().x, GetPos().y + 70), LAYER::ITEM);

			if (!IsDead())
				SetDead();
		}
	}
}
