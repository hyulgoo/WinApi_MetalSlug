#pragma once
#include "CState.h"
class CTraceSly :
    public CState
{
public:
    virtual void final_tick();
    virtual void Enter();
    virtual void Exit();

public:
    CLONE(CTraceSly);

public:
    CTraceSly();
    ~CTraceSly();
};

