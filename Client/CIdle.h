#pragma once
#include "CState.h"
class CIdle :
    public CState
{
public:
    virtual void final_tick() override;
    virtual void Enter() override {}
    virtual void Exit() override {}

public:
    CLONE(CIdle);

public:
    CIdle();
    ~CIdle();
};

