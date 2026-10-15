#include "pch.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"

#include "Game\03.Level\CLevel.h"
#include "Game\03.Level\01.Start\CStartLevel.h"
#include "Game\03.Level\02.Stage\CStage.h"
#include "Game\03.Level\03.AnimationEditor\CAnimEditor.h"
#include "Game\03.Level\01.Start\CGameOver.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

CLevelMgr::CLevelMgr()
	: m_arrLevel{}
	, m_pCurLevel(nullptr)
	, m_eCurLevel(LEVEL_TYPE::END)
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

	// 타이틀에서 시작한다. 자동 테스트는 시나리오가 시작 레벨을 정할 수 있다
	m_eCurLevel = CTestMgr::GetInst()->GetStartLevel(LEVEL_TYPE::START);
	m_pCurLevel = m_arrLevel[static_cast<UINT>(m_eCurLevel)];
	m_pCurLevel->init();
	CTestMgr::GetInst()->OnLevelEnter(m_eCurLevel);
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
	m_eCurLevel = _eNext;
	m_pCurLevel = m_arrLevel[static_cast<UINT>(_eNext)];
	m_pCurLevel->Enter();
	CTestMgr::GetInst()->OnLevelEnter(m_eCurLevel);
}
