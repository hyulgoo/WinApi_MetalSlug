#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CSoldier.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\07.MonsterState\CState.h"
#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\04.Object\Bullet\Enemy\CSLDCQC.h"
#include "Game\04.Object\Bullet\Enemy\CSLD_Grenade.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

CSoldier::CSoldier()
	: m_fCQCTime(0.f)
{
	init();
}

CSoldier::CSoldier(const CSoldier& _other)
	: m_fCQCTime(0.f)
{
	init();
}

CSoldier::~CSoldier()
{
}

void CSoldier::init()
{
	SetPos(Vec2(1000.f, 300.f));
	SetScale(Vec2(80.f, 130.f));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));

	CreatePixelCollider();
	CreateRigidbody();

	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Soldier", L"texture\\Soldier.bmp");
	LoadAnimation();

	SetMstHP(1);
	SetMstAtkDelay(1.28f);
	SetMstDeadTime(1.1f);
	SetMstSpeed(160.f);
}

void CSoldier::tick()
{
	
	CMonster::tick();

	m_pCurState = GetAI()->GetState();

	if (m_pPrevState != m_pCurState)
	{
		m_pPrevState = m_pCurState;
		Play(m_bDir);
	}

	if (m_pCurState == GetAI()->FindState(L"Attack"))
	{
		m_pCurState->SetCQC();
		m_fAttackTime += DT;

		if (m_fAttackTime > 0.8f)
		{
			CSLD_Grenade* pGrenade = new CSLD_Grenade;
			pGrenade->SetDir(m_bDir);
			Instantiate(pGrenade, Vec2(GetPos().x, GetPos().y - 50), LAYER::MONSTER_PROJECTILE);
			m_fAttackTime = 0.f;
		}
	}
	else
	{
		m_fAttackTime = 0.f;
		m_bDir = GetAI()->GetMonDir();
	}



	if (m_pCurState == GetAI()->FindState(L"AttackCQC"))
	{
		m_fCQCTime += DT;
		if (m_fCQCTime > 0.7f)
		{
			CSLDCQC* pCQC = new CSLDCQC;
			pCQC->SetDir(m_bDir);
			Instantiate(pCQC, GetPos(), LAYER::MONSTER_PROJECTILE);
			m_fCQCTime = 0.f;
		}
	}
	else
	{
		m_fCQCTime = 0.f;
	}

}

void CSoldier::render(const HDC _dc)
{
	CMonster::render(_dc);
}

void CSoldier::BeginOverlap(CCollider* _pOther)
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

void CSoldier::OnOverlap(CCollider* _pOther)
{
}

void CSoldier::LoadAnimation()
{
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_IDLE.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_BOMB.anim"); 
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_CQC.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD_FIRE.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD2_LEFT.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_RUN_LEFT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_WALK_LEFT.anim");
}

void CSoldier::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if(_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if(_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CSoldier::Play(const bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;
	if (GetAI()->FindState(L"Idle") == GetAI()->GetState())
	{
		PlayAnim(L"SOLDIER_IDLE.anim", true, bLeft);
	}
	else if (GetAI()->FindState(L"Trace") == GetAI()->GetState())
	{
		PlayAnim(L"SOLDIER_RUN_LEFT.anim", true, bLeft);
	}
	else if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		PlayAnim(L"SOLDIER_BOMB.anim", true, _bDir);
	}
	else if (GetAI()->FindState(L"Dead") == GetAI()->GetState())
	{
		if (m_bFire)
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(1);
			effect->SetFire();
			effect->SetDir(m_bDir);
			Instantiate(effect, Vec2(GetPos().x, GetPos().y + 70), LAYER::DEFAULT);
			if (!IsDead())
				SetDead();
		}
		else
		{
			CDeadEffect* effect = new CDeadEffect;
			effect->SetType(1);
			effect->SetDir(m_bDir);
			Instantiate(effect, Vec2(GetPos().x, GetPos().y + 70), LAYER::DEFAULT);
			if (!IsDead())
				SetDead();
		}
	}
	else if (GetAI()->FindState(L"TraceSly") == GetAI()->GetState())
	{
		PlayAnim(L"SOLDIER_WALK_LEFT.anim", true, bLeft);
	}
	else 	if (GetAI()->FindState(L"AttackCQC") == GetAI()->GetState())
	{
		PlayAnim(L"SOLDIER_CQC.anim", true, _bDir);
	}
}
