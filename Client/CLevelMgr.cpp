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
	for (const CLevel* level : m_arrLevel)
        DEL(level)
}

void CLevelMgr::init()
{
	// Level 생성
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::GAMEOVER)]   = new CGameOver;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::START)]      = new CStartLevel;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::STAGE)]      = new CStage;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::ANIMEDITOR)] = new CAnimEditor;

	m_pCurLevel = m_arrLevel[static_cast<UINT>(LEVEL_TYPE::STAGE)];
	m_pCurLevel->init();
}

void CLevelMgr::tick() const
{
	m_pCurLevel->tick();
	m_pCurLevel->final_tick();
}

void CLevelMgr::render(const HDC _dc) const
{
	m_pCurLevel->render(_dc);
}

void CLevelMgr::ChangeLevel(LEVEL_TYPE _eNext)
{
	assert(m_pCurLevel != m_arrLevel[static_cast<UINT>(_eNext)]);

	m_pCurLevel->Exit();
	m_pCurLevel = m_arrLevel[static_cast<UINT>(_eNext)];
	m_pCurLevel->Enter();
}
