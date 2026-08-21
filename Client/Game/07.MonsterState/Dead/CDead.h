#pragma once
#include "Game\07.MonsterState\CState.h"
class CDead :
    public CState
{
public:
    virtual void final_tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
    

public:
    CLONE(CDead);

public:
    CDead();
    ~CDead();
};

