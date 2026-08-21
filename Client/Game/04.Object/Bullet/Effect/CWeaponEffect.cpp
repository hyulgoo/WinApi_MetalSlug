#include "pch.h"
#include "Game\04.Object\Bullet\Effect\CWeaponEffect.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"

CWeaponEffect::CWeaponEffect() 
	: m_bDir()
	, m_bStart(false)
	, m_fDeadTime()
	, m_fTime()
	, m_tState()
{
	CreateAnimator();

	m_pTexture = CResMgr::GetInst()->FindTexture(L"Weapone");
	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT.anim");
	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT_HIT.anim");
	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT_WALL.anim");
	GetAnimator()->LoadAnimation(L"Player//BOMB_SFX.anim");
}

CWeaponEffect::~CWeaponEffect()
{
}

void CWeaponEffect::tick()
{
	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));
		
	if (m_bStart == false) {
		const int random = rand() % 2;
		SetPos(Vec2(GetPos().x + random * 5, GetPos().y));
		if (random == 0)
			m_bDir = true;
		else if (random == 1)
			m_bDir = false;
		if (m_tState == static_cast<STATE>(0))
		{
			m_fDeadTime = 0.3f;
			m_bStart = true;
			GetAnimator()->Play(L"BULLET_EFFECT.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_tState == static_cast<STATE>(1))
		{
			m_fDeadTime = 0.3f;
			m_bStart = true;
			GetAnimator()->Play(L"BULLET_EFFECT_HIT.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_tState == static_cast<STATE>(2))
		{
			m_fDeadTime = 0.3f;
			m_bStart = true;
			m_bDir = false;
			GetAnimator()->Play(L"BULLET_EFFECT_WALL.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_tState == static_cast<STATE>(3))
		{

			m_fDeadTime = 0.5f;
			m_bStart = true;
			GetAnimator()->Play(L"BOMB_SFX.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}		
	}

	CObj::tick();

	m_fTime += DT;
	if (m_fTime > m_fDeadTime && !IsDead())
	{
		SetDead();
	}

}


void CWeaponEffect::render(const HDC _dc)
{
	CObj::render(_dc);
}
