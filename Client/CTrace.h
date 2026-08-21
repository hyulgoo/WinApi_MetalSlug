#pragma once
#include "CState.h"
class CTrace :
    public CState
{
private:
    Vec2    m_vMontoPlayer;
public:
    virtual void final_tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
   
public:
    CLONE(CTrace);

public:
    CTrace();
    ~CTrace();
};

