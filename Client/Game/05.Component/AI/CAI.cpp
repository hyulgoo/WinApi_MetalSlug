#include "pch.h"
#include "Game\05.Component\AI\CAI.h"

#include "Game\07.MonsterState\CState.h"
#include "Game\04.Object\CObj.h"

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
	// 삭제 예정인 오브젝트는 이번 프레임 끝에 메모리가 해제되므로
	// 상태 변경 이벤트(해제될 AI 주소를 담음)를 남기지 않도록 갱신하지 않는다.
	if (nullptr == m_pCurState || GetOwner()->IsDead())
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
