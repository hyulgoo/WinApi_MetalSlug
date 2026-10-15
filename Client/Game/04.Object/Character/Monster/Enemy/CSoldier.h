#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

// 반란군 병사: 멀면 수류탄, 가까우면 천천히 다가가 칼
class CSoldier :
    public CMonster
{
public:
    virtual void              PlayStateAnim(MON_STATE _eState) override;
    virtual const CAnimation* GetAttackAnim() const override;
    virtual int               GetAttackFrame(MON_STATE _eState) const override;
    virtual void              OnAttackFrame(MON_STATE _eState) override;
    // 공격 범위 안에서 거리로 고른다: 칼 거리(0.2) 안이면 칼, 그다음(0.5)까지는 천천히 다가가기, 그 밖은 수류탄
    virtual MON_STATE         ChooseAttack(float _fDist) const override;

protected:
    virtual void SpawnDeathEffect() override;

public:
    CLONE_DEACTIVATE(CSoldier);

public:
    CSoldier();
    CSoldier(const CSoldier&) = delete;  // 복제하지 않는다
    virtual ~CSoldier() override;
};
