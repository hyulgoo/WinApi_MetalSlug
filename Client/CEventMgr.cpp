#include "pch.h"
#include "CEventMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CObj.h"
#include "CAI.h"

CEventMgr::CEventMgr()
{

}

CEventMgr::~CEventMgr()
{

}

void CEventMgr::tick()
{
	// 삭제 예정인 오브젝트 제거
	for (auto& obj : m_vecGarbage)
		delete obj;
	m_vecGarbage.clear();


	// 이벤트 처리
	for (auto& event : m_vecEvent)
    {
		switch (event.eType)
		{
		case EVENT_TYPE::CREATE_OBJECT: // wParam : Object Adress, lParam : Layer
		{
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

			CObj*       pNewObj = reinterpret_cast<CObj*>(event.wParam);
			const LAYER eLayer  = static_cast<LAYER>(event.lParam);

			pCurLevel->AddObject(pNewObj, eLayer);
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			// 삭제할 오브젝트를 Dead 상태로 만들고 Garbage 에 넣는다.
			CObj* pObj = reinterpret_cast<CObj*>(event.wParam);
			// 중복 확인: 같은 tick 안에서 동일 오브젝트에 삭제요청 한 경우 대비
			if (!pObj->m_bDead)
			{
				m_vecGarbage.push_back(pObj);
				pObj->m_bDead = true;
			}			
		}
		break;


		case EVENT_TYPE::LEVEL_CHANGE:
		{
			const LEVEL_TYPE eNextLevel = static_cast<LEVEL_TYPE>(event.wParam);
			CLevelMgr::GetInst()->ChangeLevel(eNextLevel);
		}
		break;
		case EVENT_TYPE::CHANGE_AI_STATE:
		{
			// wParam : AI Component Adress, lParam : Next State Name
			CAI*           pAI   = reinterpret_cast<CAI*>(event.wParam);
			const wchar_t* pName = reinterpret_cast<const wchar_t*>(event.lParam);
			pAI->ChangeState(pName);
		}
		break;
		}
	}

	m_vecEvent.clear();
}
