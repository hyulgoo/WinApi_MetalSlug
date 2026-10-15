#include "pch.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"

#include "Game\03.Level\CLevel.h"
#include "Game\03.Level\01.Start\CStartLevel.h"
#include "Game\03.Level\02.Stage\CStage.h"
#include "Game\03.Level\03.AnimationEditor\CAnimEditor.h"
#include "Game\03.Level\01.Start\CGameOver.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\04.CollisionMgr\CCollisionMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\09.UIMgr\CUIMgr.h"
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
        delete level;
}

void CLevelMgr::init()
{
	// Level 생성. 레벨 객체는 영구 리소스만 갖고, 오브젝트는 Enter에서 만든다
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::GAMEOVER)]   = new CGameOver;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::START)]      = new CStartLevel;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::STAGE)]      = new CStage;
	m_arrLevel[static_cast<UINT>(LEVEL_TYPE::ANIMEDITOR)] = new CAnimEditor;

	// 타이틀에서 시작한다. 자동 테스트는 시나리오가 시작 레벨을 정할 수 있다
	m_eCurLevel = CTestMgr::GetInst()->GetStartLevel(LEVEL_TYPE::START);
	m_pCurLevel = m_arrLevel[static_cast<UINT>(m_eCurLevel)];
	m_pCurLevel->Enter();
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
	// CEventMgr가 프레임 경계에서만 부른다
	if (m_pCurLevel == m_arrLevel[static_cast<UINT>(_eNext)])
		return;

	m_pCurLevel->Exit();

	// 이전 레벨의 상태가 다음 레벨로 넘어가지 않게 매니저를 비운다
	CCamera::GetInst()->Reset();
	CCollisionMgr::GetInst()->Reset();
	CUIMgr::GetInst()->Reset();
	CTimeMgr::GetInst()->ResetTimeStop();

	m_eCurLevel = _eNext;
	m_pCurLevel = m_arrLevel[static_cast<UINT>(_eNext)];
	m_pCurLevel->Enter();
	CTestMgr::GetInst()->OnLevelEnter(m_eCurLevel);
}
