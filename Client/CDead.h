#pragma once
#include "CState.h"
class CDead :
    public CState
{
public:
    virtual void final_tick();
    virtual void Enter();
    virtual void Exit();
    

public:
    CLONE(CDead);

public:
    CDead();
    ~CDead();
};

