#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

CDeadEffect::CDeadEffect()
	: m_pAtlas(nullptr)
	, m_bDir(false)
	, m_fDeadTime()
	, m_fTime()
	, m_bFire(false)
	, m_iType(1)
{
	CreateAnimator();
	CreatePixelCollider();
	CreateRigidbody();
	m_irand = rand() % 2;
}

CDeadEffect::~CDeadEffect()
{
}

void CDeadEffect::tick()
{
	if (m_fTime == 0)
	{
		if (m_iType == 1)
			m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Soldier", L"texture\\Soldier.bmp");
		else if (m_iType == 2)
			m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
		else if (m_iType < 1 || m_iType > 2)
			assert(false);
		LoadAnimation();
		Play(m_bDir);
	}


	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4 * DT, GetPos().y));

	m_fTime += DT;

	if (m_fTime > m_fDeadTime)
	{
		if (!IsDead())
			SetDead();
	}

	CObj::tick();
}

void CDeadEffect::render(const HDC _dc)
{
	CObj::render(_dc);
}

void CDeadEffect::LoadAnimation()
{
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD_FIRE.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD2_LEFT.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CDeadEffect::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CDeadEffect::Play(const bool _bDir)
{
	bool bLeft;
	if (_bDir)
		bLeft = false;
	else if (_bDir == false)
		bLeft = true;
	if (m_iType == 1)
	{
		if (m_bFire)
		{
				m_fDeadTime = 2.2f;
			PlayAnim(L"SOLDIER_DEAD_FIRE.anim", true, bLeft);
		}
		else
		{
				m_fDeadTime = 1.1f;
			if(m_irand == 0)
				PlayAnim(L"SOLDIER_DEAD.anim", true, bLeft);
			else if(m_irand == 1)
				PlayAnim(L"SOLDIER_DEAD2_LEFT.anim", true, bLeft);
		}
	}
	else if (m_iType == 2)
	{
		if (m_bFire)
		{
				m_fDeadTime = 4.62f;
			PlayAnim(L"ZB_DEAD_FIRE.anim", false, bLeft);
		}
		else
		{
				m_fDeadTime = 0.6f;
			PlayAnim(L"ZB_DEAD_NORMAL.anim", false, bLeft);
		}
	}
}

