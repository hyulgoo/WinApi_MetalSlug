#pragma once
#include "CState.h"
class CIdle :
    public CState
{
public:
    virtual void final_tick();
    virtual void Enter() {}
    virtual void Exit() {}

public:
    CLONE(CIdle);

public:
    CIdle();
    ~CIdle();
};

