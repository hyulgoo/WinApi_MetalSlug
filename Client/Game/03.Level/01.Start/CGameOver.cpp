#include "pch.h"
#include "Game\03.Level\01.Start\CGameOver.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CGameOver::CGameOver()
	: m_pAtlas(nullptr)
	, m_fTime(0.f)
	, m_fHoldTime(0.f)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"GAMEOVER", L"texture\\GAMEOVER.bmp");
}

CGameOver::~CGameOver()
{
}

void CGameOver::tick()
{
	if(IsPressed(KEY::C))
	{
		m_fTime += DT;

		if(m_fTime > m_fHoldTime)
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
	// C를 누르고 있던 시간은 들어올 때마다 처음부터 센다
	const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"title", L"gameover");
	m_fHoldTime               = tData.GetFloat(L"hold_time");
	m_fTime                   = 0.f;
	CCamera::GetInst()->FadeIn(tData.GetFloat(L"fade_in"));
}

void CGameOver::Exit()
{
	DeleteAllObjects();
}

