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
	 // 마우스 좌클릭 버튼 인식
	 const bool bLbtnPressed = IsTap(KEY::LBTN);
	 const bool bLbtnReleased = IsRelease(KEY::LBTN);

	 // 현재 레벨을 가져와서 모든 UI 중 우선 클릭 대상을 검사
	 CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	 const vector<CObj*>& vecUI = pCurLevel->GetLayer(LAYER::UI);
	 
	 for (int i = static_cast<int>(vecUI.size()) - 1; 0 <= i; --i)
	 {
	 	m_pPriorityUI = GetPriorityUI(dynamic_cast<CUI*>(vecUI[i]));
	 
	 	if (nullptr == m_pPriorityUI)
	 		continue;
	 
	 	m_pPriorityUI->MouseOn();
	 
	 		// UI를 클릭한 경우 Down을 true로
	 	if (bLbtnPressed)
	 	{
	 		m_pPriorityUI->MouseLbtnDown();
	 
	 		m_pFocusedUI = dynamic_cast<CUI*>(vecUI[i]);
	 		pCurLevel->SetFocusedUI(m_pFocusedUI);
	 		break;
	 	}
	 
	 	if (bLbtnReleased)
	 	{
	 		// UI에서 마우스를 뗐을 때 눌려 있었다면 Click 호출
	 		if (m_pPriorityUI->m_bLbtnDown)
	 		{
	 			m_pPriorityUI->MouseLbtnClicked();
	 			m_pPriorityUI->m_bLbtnDown = false;
	 		}
	 	}
	 }
}

 // 다른 UI들 중에서 우선적으로 이벤트를 호출할 UI 구하기
CUI* CUIMgr::GetPriorityUI(CUI* _pParentUI) const
{
	const bool bLbtnReleased = IsRelease(KEY::LBTN);

	CUI* pPriorityUI = nullptr;

	// queue에 부모 UI를 삽입
	static list<CUI*> queue;
	queue.clear();
	queue.push_back(_pParentUI);

	while (!queue.empty())
	{
		CUI* pUI = queue.front();
		queue.pop_front();

		// 해당 UI의 자식UI를 가져옴
	    // 새로 찾은 ChildUI를 queue에 push_back하고
	    // 해당 UI도 나중에 while문에서 꺼내어 처리한다.
		for (CUI* ui : pUI->GetChildUI())
			queue.push_back(ui);
		// 해당 위치에 마우스가 올라와 있고
		//  이전에 저장돼 있던 우선순위 UI가 눌린 상태였다면 해제하고 해당 UI를 우선순위 UI로 설정
		// *나중에 확인되는 UI일수록 자식쪽에 가까운 UI이기 때문에 우선순위로 취급*
		if (pUI->IsMouseOn())
		{
			if (bLbtnReleased && nullptr != pPriorityUI && pPriorityUI->IsLbtnDown())
			{
				pPriorityUI->m_bLbtnDown = false;
			}
			pPriorityUI = pUI;
		}
		// (우선순위가 아닌) 마우스가 올라와 있지 않은 상태에서 눌려 있던 버튼은 뗄 때 해제되게 함.
		else if (bLbtnReleased)
		{
			pUI->m_bLbtnDown = false;
		}
	}

	return pPriorityUI;
}
