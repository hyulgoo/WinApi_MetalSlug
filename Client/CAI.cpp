#include "pch.h"
#include "CAI.h"

#include "CState.h"

CAI::CAI(CObj* _pOWner)
	: CComponent(_pOWner)
	, m_pCurState(nullptr)
{
}

CAI::~CAI()
{
    for (auto& iter : m_mapState)
        DEL(iter.second);
    
	m_mapState.clear();
}

void CAI::final_tick()
{
	if (nullptr == m_pCurState)
		return;

	m_pCurState->final_tick();
}

void CAI::AddState(const wstring& _strKey, CState* _pState)
{
	m_mapState.insert(make_pair(_strKey, _pState));
	_pState->m_pOwnerAI = this;
}

bool CAI::GetMonDir() const
{
	if (m_pCurState->GetMontoPlayer().x >= 0)
		return true;
    
	return false; 
}

void CAI::ChangeState(const wstring& _strStateName)
{
	CState* pNextState = FindState(_strStateName);
	assert(pNextState);

	if (nullptr != m_pCurState)
		m_pCurState->Exit();

	m_pCurState = pNextState;

	m_pCurState->Enter();
}
