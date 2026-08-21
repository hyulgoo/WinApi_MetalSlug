#include "pch.h"
#include "CState.h"

#include "CEventMgr.h"

CState::CState()
	: m_pOwnerAI(nullptr)
{
}

CState::~CState()
{
}

void CState::ChangeState(const wchar_t* _pStateName)
{
	tEvent evn = {};

	evn.eType = EVENT_TYPE::CHANGE_AI_STATE;
	evn.wParam = (DWORD_PTR)GetOwnerAI();
	evn.lParam = (DWORD_PTR)_pStateName;

	CEventMgr::GetInst()->AddEvent(evn);
}
