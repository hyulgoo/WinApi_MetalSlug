#include "pch.h"
#include "CWeaponeEffect.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include <ctime>

CWeaponeEffect::CWeaponeEffect() 
	: m_bDir()
	, m_tState()
	, m_bStart(false)
	, m_fDeadTime()
	, m_fTime()
{
	CreateAnimator();

	m_pTexture = CResMgr::GetInst()->FindTexture(L"Weapone");
	GetAnimator()->LoadAnimaton(L"Player//BULLET_EFFECT.anim");
	GetAnimator()->LoadAnimaton(L"Player//BULLET_EFFECT_HIT.anim");
	GetAnimator()->LoadAnimaton(L"Player//BULLET_EFFECT_WALL.anim");
	GetAnimator()->LoadAnimaton(L"Player//BOMB_SFX.anim");
}

CWeaponeEffect::~CWeaponeEffect()
{
}

void CWeaponeEffect::tick()
{
	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));
		
	if (m_bStart == false) {
		int random = rand() % 2;
		SetPos(Vec2(GetPos().x + random * 5, GetPos().y));
		if (random == 0)
			m_bDir = true;
		else if (random == 1)
			m_bDir = false;
		if (m_tState == (STATE)0)
		{
			m_fDeadTime = 0.3f;
			m_bStart = true;
			GetAnimator()->Play(L"BULLET_EFFECT.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_tState == (STATE)1)
		{
			m_fDeadTime = 0.3f;
			m_bStart = true;
			GetAnimator()->Play(L"BULLET_EFFECT_HIT.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_tState == (STATE)2)
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
		else if (m_tState == (STATE)3)
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


void CWeaponeEffect::render(HDC _dc)
{
	CObj::render(_dc);
}
