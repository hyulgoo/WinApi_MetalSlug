#include "pch.h"
#include "CUIMgr.h"

#include "CKeyMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CUI.h"

CUIMgr::CUIMgr()
	: m_pFocusedUI(nullptr)
	, m_pPriorityUI(nullptr)
{

}
CUIMgr::~CUIMgr()
{

}
void CUIMgr::tick()
{
	 // 왼쪽클릭버튼 인식
	 bool bLbtnPressed = IsTap(KEY::LBTN);
	 bool bLbtnReleased = IsRelease(KEY::LBTN);
	 
	 // 현재레벨을 가져와서 모든 UI에 대해 클릭유무 검사
	 CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	 const vector<CObj*>& vecUI = pCurLevel->GetLayer(LAYER::UI);
	 
	 for (int i = (int)vecUI.size() - 1; 0 <= i; --i)
	 {
	 	m_pPriorityUI = GetPriorityUI((CUI*)vecUI[i]);
	 
	 	if (nullptr == m_pPriorityUI)
	 		continue;
	 
	 	m_pPriorityUI->MouseOn();
	 
	 		// UI 위에서 누르면 Down이 true로
	 	if (bLbtnPressed)
	 	{
	 		m_pPriorityUI->MouseLbtnDown();
	 
	 		m_pFocusedUI = (CUI*)vecUI[i];
	 		pCurLevel->SetFocusedUI(m_pFocusedUI);
	 		break;
	 	}
	 
	 	else if (bLbtnReleased)
	 	{ 
	 		// UI 위에서 떼졌는데 이전에 눌렸다면 Click 호출
	 		if (m_pPriorityUI->m_bLbtnDown)
	 		{
	 			m_pPriorityUI->MouseLbtnClicked();
	 			m_pPriorityUI->m_bLbtnDown = false;
	 		}
	 	}
	 }
}

 // 다른 UI보다 우선적으로 이벤트가 호출될 UI 정하기
CUI* CUIMgr::GetPriorityUI(CUI* _pParentUI)
{
	bool bLbtnReleased = IsRelease(KEY::LBTN);

	CUI* pPriorityUI = nullptr;

	// queue에 부모UI를 넣음
	static list<CUI*> queue;
	queue.clear();
	queue.push_back(_pParentUI);

	while (!queue.empty())
	{
		CUI* pUI = queue.front();
		queue.pop_front();

		// 해당 UI의 자식UI를 가져옴
		const vector<CUI*>& vecChild = pUI->GetChildUI();
		for (size_t i = 0; i < vecChild.size(); ++i)
		{
			// 새로운 ChildUI vector를 만들어 push_back하고
			// 해당 vector를 pUI로 지정함.
			queue.push_back(vecChild[i]);
		}
		// 해당위치에 마우스가 올라가있고 마우스가
		//  떼지고 이전에 있던 우선순위가 눌린상태였다면 해제하고 해당 UI를 우선순위 UI로 지정
		// *나중에 확인되는 UI일수록 자식계층이 낮은 UI이기 때문에 우선순위가 높음*
		if (pUI->IsMouseOn())
		{
			if (bLbtnReleased && nullptr != pPriorityUI && pPriorityUI->IsLbtnDown())
			{
				pPriorityUI->m_bLbtnDown = false;
			}
			pPriorityUI = pUI;
		}
		// (우선순위가 아닌)마우스가 올라가있지 않은 상태에서 눌린 버튼은 다 해제되게 함.
		else if (bLbtnReleased)
		{
			pUI->m_bLbtnDown = false;
		}
	}

	return pPriorityUI;
}
