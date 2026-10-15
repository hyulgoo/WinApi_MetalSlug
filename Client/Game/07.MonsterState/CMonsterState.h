#pragma once

class CMonster;

// 몬스터 상태. 상태가 그 상태의 애니메이션(Enter)과 행동(Update)을 맡는다.
// 상태 객체는 데이터가 없는 공용 객체이고(몬스터마다 만들지 않는다), 속도·범위·애니메이션·공격 같은
// 몬스터마다 다른 것은 몬스터에게 묻는다
class CMonsterState
{
public:
    virtual void Enter(CMonster& /*_mon*/) const {}
    virtual void Update(CMonster& _mon) const = 0;   // CAI::final_tick마다
    virtual void Exit(CMonster& /*_mon*/) const {}

    // 상태 ID의 공용 상태 객체
    static const CMonsterState* Get(MON_STATE _eState);

    CMonsterState() = default;
    CMonsterState(const CMonsterState&) = delete;
    CMonsterState& operator=(const CMonsterState&) = delete;
    virtual ~CMonsterState() = default;
};
