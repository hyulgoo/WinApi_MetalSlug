#pragma once
#include "CState.h"
class CAttackCQC :
    public CState
{
private:
    float   m_fAttackDelay;
    bool    m_bDetail;

public:
    virtual void final_tick();
    virtual void Enter();
    virtual void Exit();

public:
    CLONE(CAttackCQC);

public:
    CAttackCQC();
    ~CAttackCQC();
};
