#include "pch.h"
#include "CEventMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CObj.h"
#include "CAI.h"
#include "CState.h"

CEventMgr::CEventMgr()
{

}

CEventMgr::~CEventMgr()
{

}

void CEventMgr::tick()
{
	// 삭제예정 오브젝트 삭제
	for (size_t i = 0; i < m_vecGarbage.size(); ++i)
	{
		delete m_vecGarbage[i];
	}
	m_vecGarbage.clear();


	// 이벤트 처리
	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJECT: // wParam : Object Adress, lParam : Layer
		{
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

			CObj* pNewObj = (CObj*)m_vecEvent[i].wParam;
			LAYER eLayer = (LAYER)m_vecEvent[i].lParam;

			pCurLevel->AddObject(pNewObj, eLayer);
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			// 삭제예정 오브젝트를 Dead 상태로 두고 Garbage 에 넣는다.
			CObj* pObj = (CObj*)m_vecEvent[i].wParam;
			// 낮을 확률로 같은 tick 에서 동시에 같은 오브젝트를 삭제요청 한 경우를 방어
			if (!pObj->m_bDead)
			{
				m_vecGarbage.push_back(pObj);
				pObj->m_bDead = true;
			}			
		}
		break;


		case EVENT_TYPE::LEVEL_CHANGE:
		{
			LEVEL_TYPE eNextLevel = (LEVEL_TYPE)m_vecEvent[i].wParam;
			CLevelMgr::GetInst()->ChangeLevel(eNextLevel);
		}
		break;
		case EVENT_TYPE::CHANGE_AI_STATE:
		{
			// wParam : AI Component Adress, lParam : Next State Name
			CAI* pAI = (CAI*)m_vecEvent[i].wParam;
			const wchar_t* pName = (const wchar_t*)m_vecEvent[i].lParam;
			pAI->ChangeState(pName);
		}
		break;
		}
	}

	m_vecEvent.clear();
}
