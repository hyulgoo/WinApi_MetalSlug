#include "pch.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CStartLevel.h"
#include "CStage.h"
#include "CAnimEditor.h"
#include "CGameOver.h"

CLevelMgr::CLevelMgr()
	: m_arrLevel{}
	, m_pCurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	for (int i = 0; i < (int)LEVEL_TYPE::END; ++i)
		DEL(m_arrLevel[i]);		
}

void CLevelMgr::init()
{
	// Level »ý¼º
	m_arrLevel[(UINT)LEVEL_TYPE::GAMEOVER] = new CGameOver;
	m_arrLevel[(UINT)LEVEL_TYPE::START] = new CStartLevel;
	m_arrLevel[(UINT)LEVEL_TYPE::STAGE] = new CStage;
	m_arrLevel[(UINT)LEVEL_TYPE::ANIMEDITOR] = new CAnimEditor;

	m_pCurLevel = m_arrLevel[(UINT)LEVEL_TYPE::ANIMEDITOR];
	m_pCurLevel->init();
}

void CLevelMgr::tick()
{
	m_pCurLevel->tick();
	m_pCurLevel->final_tick();
}

void CLevelMgr::render(HDC _dc)
{
	m_pCurLevel->render(_dc);
}

void CLevelMgr::ChangeLevel(LEVEL_TYPE _eNext)
{
	assert(m_pCurLevel != m_arrLevel[(UINT)_eNext]);

	m_pCurLevel->Exit();
	m_pCurLevel = m_arrLevel[(UINT)_eNext];
	m_pCurLevel->Enter();
}
