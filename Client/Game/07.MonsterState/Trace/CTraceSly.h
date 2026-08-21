#pragma once
#include "Game\07.MonsterState\CState.h"
class CTraceSly :
    public CState
{
public:
    virtual void final_tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CLONE(CTraceSly);

public:
    CTraceSly();
    ~CTraceSly();
};

