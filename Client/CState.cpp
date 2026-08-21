#include "pch.h"
#include "CState.h"

#include "CEventMgr.h"

CState::CState()
    : m_pOwnerAI(nullptr)
    , m_bCQC(false)
{
}

CState::~CState()
{
}

void CState::ChangeState(const wchar_t* _pStateName) const
{
	tEvent evn;
	evn.eType  = EVENT_TYPE::CHANGE_AI_STATE;
	evn.wParam = reinterpret_cast<DWORD_PTR>(GetOwnerAI());
	evn.lParam = reinterpret_cast<DWORD_PTR>(_pStateName);

	CEventMgr::GetInst()->AddEvent(evn);
}
