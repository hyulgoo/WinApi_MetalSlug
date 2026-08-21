#include "pch.h"
#include "CGameOver.h"
#include "CEngine.h"
#include "CCamera.h"
#include "CResMgr.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CTexture.h"

CGameOver::CGameOver()
	: m_pAtlas(nullptr)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"GAMEOVER", L"texture\\GAMEOVER.bmp");
}

CGameOver::~CGameOver()
{
}

void CGameOver::init()
{
	CCamera::GetInst()->FadeIn(7.f);
}

void CGameOver::tick()
{
	if(IsPressed(KEY::C))
	{
		m_fTime += DT;

		if(m_fTime > 2.f)
			ChangeLevel(LEVEL_TYPE::START);
	}

	CLevel::tick();
}

void CGameOver::render(const HDC _dc)
{
	StretchBlt(_dc
		, 0, 0
		, static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
		, m_pAtlas->GetDC()
		, 0
		, 0
		, static_cast<int>(m_pAtlas->Width())
		, static_cast<int>(m_pAtlas->Height()), SRCCOPY);
	CLevel::render(_dc);
}

void CGameOver::Enter()
{
	init();
}

void CGameOver::Exit()
{
	DeleteObject();
}

