#include "pch.h"
#include "Game\02.Manager\05.EventMgr\CEventMgr.h"

#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\04.CollisionMgr\CCollisionMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\CObj.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

CEventMgr::CEventMgr()
{

}

CEventMgr::~CEventMgr()
{
	// 종료할 때 남은 요청과 지울 오브젝트를 정리한다 (누수 보고에 남지 않게)
	DiscardEvents(m_vecEvent);
	DeleteGarbage();
}

void CEventMgr::tick()
{
	// 지난 프레임에 삭제 표시한 오브젝트를 해제한다. 레이어에서는 이미 빠져 있다
	DeleteGarbage();

	// 이번 프레임에 쌓인 요청을 떼어 내서 처리한다.
	// 처리 도중 추가되는 요청(레벨 Enter가 만드는 오브젝트 등)은 m_vecEvent에 새로 쌓여 다음 프레임에 처리된다
	vector<tEvent> vecEvent;
	vecEvent.swap(m_vecEvent);

	bool       bLevelChange = false;
	LEVEL_TYPE eNextLevel   = LEVEL_TYPE::END;

	for (const tEvent& event : vecEvent)
	{
		switch (event.eType)
		{
		case EVENT_TYPE::CREATE_OBJECT: // wParam : Object Adress, lParam : Layer
		{
			CLevel*     pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
			CObj*       pNewObj   = reinterpret_cast<CObj*>(event.wParam);
			const LAYER eLayer    = static_cast<LAYER>(event.lParam);

			pCurLevel->AddObject(pNewObj, eLayer);
			CTestMgr::GetInst()->OnObjectCreated(pNewObj, eLayer);
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			// 삭제 표시를 하고 레이어에서 뺀 뒤 Garbage에 넣는다.
			// 같은 프레임에 같은 오브젝트를 두 번 요청한 경우는 표시로 걸러진다
			CObj* pObj = reinterpret_cast<CObj*>(event.wParam);
			if (!pObj->m_bDead)
			{
				pObj->m_bDead = true;
				CCollisionMgr::GetInst()->ReleaseObject(pObj);
				CLevelMgr::GetInst()->GetCurLevel()->RemoveObject(pObj);
				m_vecGarbage.push_back(pObj);
			}
		}
		break;
		case EVENT_TYPE::LEVEL_CHANGE:
		{
			// 이번 프레임의 다른 요청을 모두 처리한 뒤에 바꾼다
			bLevelChange = true;
			eNextLevel   = static_cast<LEVEL_TYPE>(event.wParam);
		}
		break;
		}
	}

	if (!bLevelChange)
		return;

	// 위 처리 도중 이전 레벨이 남긴 요청은 새 레벨에 들어가면 안 되므로 버린다
	DiscardEvents(m_vecEvent);

	// 프레임 경계에서 레벨을 바꾼다. 삭제 표시된 오브젝트는 레이어에 없으므로 Exit가 다시 지우지 않는다
	CLevelMgr::GetInst()->ChangeLevel(eNextLevel);
}

void CEventMgr::DeleteGarbage()
{
	for (CObj* pObj : m_vecGarbage)
	{
		CTestMgr::GetInst()->OnObjectDeleted(pObj);
		delete pObj;
	}
	m_vecGarbage.clear();
}

void CEventMgr::DiscardEvents(vector<tEvent>& _vecEvent)
{
	// 아직 레벨에 들어가지 않은 생성 요청의 오브젝트는 여기서 지운다
	for (const tEvent& event : _vecEvent)
	{
		if (EVENT_TYPE::CREATE_OBJECT == event.eType)
			delete reinterpret_cast<CObj*>(event.wParam);
	}
	_vecEvent.clear();
}
