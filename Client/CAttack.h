#pragma once
#include "CState.h"
class CAttack :
    public CState
{
private:
    float   m_fAttackDelay;
    bool    m_bNowAttack;

public:
    virtual void final_tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CLONE(CAttack);

public:
    CAttack();
    ~CAttack();
};

