#pragma once
#include "CState.h"
class CTrace :
    public CState
{
private:
    Vec2    m_vMontoPlayer;
public:
    virtual void final_tick();
    virtual void Enter();
    virtual void Exit();
   
public:
    CLONE(CTrace);

public:
    CTrace();
    ~CTrace();
};

