#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie_RSH.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\04.Object\Bullet\Enemy\CZB_Bullet.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"
#include "Game\04.Object\EventBox\CItem.h"

namespace
{
	// 공격 애니메이션(ZB_RCH_ATTACK.anim)이 이 프레임에 들어설 때 토사물을 쏜다.
	// 예전 1.82초 타이머에 가장 가까운 프레임 시작(15번, 1.80초)
	constexpr int ATTACK_FIRE_FRAME = 15;
}

CZombie_RSH::CZombie_RSH()
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
		// 예전에는 Attack 상태에 들어온 뒤 흐른 시간으로 쐈다. 애니메이션 재생 속도와 따로 놀아 FPS가 바뀌면
		// 토하는 동작과 발사가 어긋났다. 공격 애니메이션의 프레임에 묶어 Attack에 들어올 때마다 한 번 쏜다
		const CAnimation* pAttackAnim = GetAnimator2()->GetAnimation();
		if (nullptr != pAttackAnim && pAttackAnim->IsFrameEntered(ATTACK_FIRE_FRAME))
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
		}
	}
	else
	{
		// 공격 중이 아닐 때 플레이어 쪽으로 방향을 갱신한다 (이 줄이 노인·연구원 좀비에만 빠져 있었다)
		m_bDir = GetAI()->GetMonDir();
	}
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

void CZombie_RSH::OnOverlap(CCollider* /*_pOther*/)
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
	const bool bLeft = !_bDir;

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
