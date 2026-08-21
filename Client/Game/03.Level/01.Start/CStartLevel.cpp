#include "pch.h"
#include "Game\03.Level\01.Start\CStartLevel.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\04.Object\CObj.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\03.Level\01.Start\CSeletAnim.h"
#include "Game\04.Object\BackGround\CStartBG.h"

CStartLevel::CStartLevel()
	: m_bStart(false)
	, m_pAtlas(nullptr)
	, m_fTime(0.f)
	, m_bEnd(false)
{
	CStartBG* pStartBG = new CStartBG;
	pStartBG->SetPos(Vec2(0.f, 0.f));
	AddObject(pStartBG, LAYER::BACKGROUND);

	m_pSelectAnim = new CSeletAnim;
	m_pSelectAnim->SetPos(Vec2(340.f, 230.f));
	AddObject(m_pSelectAnim, LAYER::PLAYER);	
}

CStartLevel::~CStartLevel()
{
}

void CStartLevel::init()
{	
}

void CStartLevel::tick()
{
	if (IsTap(KEY::C))
	{
		m_pSelectAnim->SetStart();
		CCamera::GetInst()->CameraShake(300.f, 200.f, 1.5f);
		m_bStart = true;
	}

	if (IsTap(KEY::LEFT) || IsTap(KEY::RIGHT))
		m_pSelectAnim->SetNumber();

	if (m_bStart)
	{
		m_fTime += DT;

		if (m_fTime > 0.5f)
		{
			if (!m_bEnd)
			{
				CCamera::GetInst()->FadeOut(1.f);
				m_bEnd = true;
			}
		}

		if (m_fTime > 1.5f)
			ChangeLevel(LEVEL_TYPE::STAGE);
	}

	CLevel::tick();
}

void CStartLevel::Enter()
{
	init();
}

void CStartLevel::Exit()
{
	DeleteObject();
}

