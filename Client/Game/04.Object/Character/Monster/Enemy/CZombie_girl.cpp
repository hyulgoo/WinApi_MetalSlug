#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie_girl.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\04.Object\Bullet\Enemy\CZB_Bullet.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

CZombie_girl::CZombie_girl()
{
	init();
}

CZombie_girl::CZombie_girl(const CZombie_girl& _other)
{
	init();
}

CZombie_girl::~CZombie_girl()
{
}

void CZombie_girl::init()
{
	SetPos(Vec2(1000.f, 300.f));
	SetScale(Vec2(60.f, 130.f));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));

	CreatePixelCollider();
	CreateRigidbody();
	CreateAnimator2();

	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
	LoadAnimation();

	SetMstHP(10);
	SetMstAtkDelay(2.73f);
	SetMstAtkRange(300.f);
	SetMstSpeed(80.f);
}

void CZombie_girl::tick()
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
				Offset = 70.f;
			}
			else
			{
				ZBAT->SetDir(true);
				Offset = -70.f;
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
	{
		m_fAttackTime = 0.f;
		m_bDir = GetAI()->GetMonDir();
	}
}

void CZombie_girl::render(const HDC _dc)
{

	CMonster::render(_dc);
}

void CZombie_girl::BeginOverlap(CCollider* _pOther)
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

void CZombie_girl::OnOverlap(CCollider* _pOther)
{
}

void CZombie_girl::LoadAnimation()
{
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_NOTHING.anim");
	GetAnimator2()->LoadAnimation(L"Enemy//ZB_GIRL_ATTACK.anim"); 
	GetAnimator()->LoadAnimation(L"Enemy//ZB_GIRL_ATTACK_EFFECT.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_GIRL_IDLE.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_GIRL_RUN.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CZombie_girl::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CZombie_girl::PlayEffect(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator2()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator2()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CZombie_girl::Play(const bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;
	if (GetAI()->FindState(L"Idle") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_GIRL_IDLE.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Trace") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_GIRL_RUN.anim", true, bLeft);
		PlayEffect(L"ZB_NOTHING.anim", false, bLeft);
	}
	else if (GetAI()->FindState(L"Attack") == GetAI()->GetState())
	{
		PlayAnim(L"ZB_GIRL_ATTACK_EFFECT.anim", true, bLeft);
		PlayEffect(L"ZB_GIRL_ATTACK.anim", false, bLeft);
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
