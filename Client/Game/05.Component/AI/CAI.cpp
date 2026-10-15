#include "pch.h"
#include "Game\05.Component\AI\CAI.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\04.Object\Character\Monster\CMonster.h"
#include "Game\07.MonsterState\CMonsterState.h"

CAI::CAI(CObj* _pOwner)
    : CComponent(_pOwner)
{
    // AI는 몬스터만 만든다 (CMonster 생성자)
    assert(nullptr != dynamic_cast<CMonster*>(_pOwner));
}

CAI::~CAI()
{
}

void CAI::AddState(const MON_STATE _eState)
{
    m_arrState[static_cast<UINT>(_eState)] = CMonsterState::Get(_eState);
}

void CAI::ChangeState(const MON_STATE _eNext)
{
    const CMonsterState* pNext = MON_STATE::END == _eNext ? nullptr : m_arrState[static_cast<UINT>(_eNext)];
    if (nullptr == pNext)
    {
        ReportError(L"몬스터 상태 전환 실패", L"이 몬스터에 등록하지 않은 상태로 바꾸려 했다");
        return;
    }

    CMonster& mon = *static_cast<CMonster*>(GetOwner());
    if (MON_STATE::END != m_eState)
        m_arrState[static_cast<UINT>(m_eState)]->Exit(mon);

    m_eState     = _eNext;
    m_fStateTime = 0.f;
    pNext->Enter(mon);
}

void CAI::final_tick()
{
    CMonster& mon = *static_cast<CMonster*>(GetOwner());

    // 지워질 몬스터는 더 갱신하지 않는다
    if (MON_STATE::END == m_eState || mon.IsDead())
        return;

    // HP가 다하면 어느 상태에서든 죽는다 (예전에는 이 검사가 다섯 상태에 따로 있었다)
    if (MON_STATE::DEAD != m_eState && mon.GetHP() < 1)
    {
        ChangeState(MON_STATE::DEAD);
        return;
    }

    m_fStateTime += DT;
    m_arrState[static_cast<UINT>(m_eState)]->Update(mon);
}
