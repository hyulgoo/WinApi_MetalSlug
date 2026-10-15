#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CSoldier.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include "Game\07.MonsterState\CState.h"
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
}

CSoldier::CSoldier()
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

	// 수류탄·칼은 공격 애니메이션의 프레임에 묶어 상태에 들어올 때마다 한 번 낸다.
	// 예전에는 상태에 들어온 뒤 흐른 시간(0.8초·0.7초)으로 냈다. 근접 공격 상태는 1.6초라
	// 0.7초 타이머가 1.4초에 한 번 더 돌아, 칼을 거두는 동작에서 두 번째 공격이 나갔다
	const CAnimation* pAnim = GetAnimator()->GetAnimation();

	if (m_pCurState == GetAI()->FindState(L"Attack"))
	{
		m_pCurState->SetCQC();

		if (nullptr != pAnim && pAnim->IsFrameEntered(GRENADE_THROW_FRAME))
		{
			CSLD_Grenade* pGrenade = new CSLD_Grenade;
			pGrenade->SetDir(m_bDir);
			Instantiate(pGrenade, Vec2(GetPos().x, GetPos().y - 50), LAYER::MONSTER_PROJECTILE);
		}
	}
	else
	{
		m_bDir = GetAI()->GetMonDir();
	}

	if (m_pCurState == GetAI()->FindState(L"AttackCQC"))
	{
		if (nullptr != pAnim && pAnim->IsFrameEntered(KNIFE_HIT_FRAME))
		{
			CSLDCQC* pCQC = new CSLDCQC;
			pCQC->SetDir(m_bDir);
			Instantiate(pCQC, GetPos(), LAYER::MONSTER_PROJECTILE);
		}
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

void CSoldier::OnOverlap(CCollider* /*_pOther*/)
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
	const bool bLeft = !_bDir;
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
