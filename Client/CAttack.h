#pragma once
#include "CState.h"
class CAttack :
    public CState
{
private:
    float   m_fAttackDelay;
    bool    m_bNowAttack;

public:
    virtual void final_tick();
    virtual void Enter();
    virtual void Exit();

public:
    CLONE(CAttack);

public:
    CAttack();
    ~CAttack();
};

