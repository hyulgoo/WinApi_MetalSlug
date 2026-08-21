#include "pch.h"
#include "CZB_PRJ_Effect.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CAnimation.h"


CZB_PRJ_Effect::CZB_PRJ_Effect()
	: m_bDir()
	, m_bState()
	, m_bStart(false)
	, m_fDeadTime()
	, m_fTime()

{
	CreateAnimator();

	m_pTexture = CResMgr::GetInst()->FindTexture(L"Zombie");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_PROJECTILE_GROUND.anim");
	GetAnimator()->LoadAnimaton(L"Enemy//ZB_PROJECTILE_HIT.anim");
}

CZB_PRJ_Effect::~CZB_PRJ_Effect()
{
}

void CZB_PRJ_Effect::tick()
{
	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

	if (m_bStart == false)
	{
		if (m_bState)
		{
			m_fDeadTime = 1.5f;
			m_bStart = true;
			GetAnimator()->Play(L"ZB_PROJECTILE_GROUND.anim", false);
			if (m_bDir)
				GetAnimator()->GetAnimation()->SetPlayRight();
			else
				GetAnimator()->GetAnimation()->SetPlayLeft();
		}
		else if (m_bState == false)
		{
			m_fDeadTime = 0.84f;
			m_bStart = true;
			GetAnimator()->Play(L"ZB_PROJECTILE_HIT.anim", false);
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

void CZB_PRJ_Effect::render(HDC _dc)
{
	CObj::render(_dc);
}
