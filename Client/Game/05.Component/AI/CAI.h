#pragma once
#include "Game\05.Component\CComponent.h"

class CMonsterState;

// 몬스터 FSM(대표 기능)
// 상태는 enum(MON_STATE)으로 고르고, 공용 상태 객체(07.MonsterState)가 그 상태의 애니메이션과 행동을 맡는다.
// 몬스터마다 쓰는 상태만 등록한다(좀비는 IDLE·TRACE·ATTACK·DEAD, 병사는 여기에 TRACE_SLY·ATTACK_CQC).
// 예전에는 상태 키가 문자열이라 오타가 Release에서 null 역참조가 됐고, 모든 몬스터가 상태 여섯 개를 따로 만들었으며,
// 상태 전환을 이벤트로 미뤄 몬스터가 다음 프레임에 상태를 거꾸로 찾아 애니메이션을 틀었다
class CAI :
    public CComponent
{
private:
    const CMonsterState* m_arrState[static_cast<UINT>(MON_STATE::END)] = {};
    MON_STATE            m_eState     = MON_STATE::END;
    float                m_fStateTime = 0.f;    // 지금 상태에 들어온 뒤 흐른 시간

public:
    virtual void tick() override {}
    virtual void final_tick() override;
    virtual void render(HDC /*_dc*/) override {}

public:
    void      AddState(MON_STATE _eState);      // 이 몬스터가 쓰는 상태로 등록한다
    void      ChangeState(MON_STATE _eNext);    // 바로 바꾼다(Exit -> Enter). 같은 상태로 바꾸면 처음부터 다시
    MON_STATE GetState() const { return m_eState; }
    float     GetStateTime() const { return m_fStateTime; }

    CLONE_DEACTIVATE(CAI);

public:
    CAI(CObj* _pOwner);
    CAI(const CAI&) = delete;
    ~CAI();
};
