#pragma once
#include "CState.h"
class CAttackCQC :
    public CState
{
private:
    float   m_fAttackDelay;
    bool    m_bDetail;

public:
    virtual void final_tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CLONE(CAttackCQC);

public:
    CAttackCQC();
    ~CAttackCQC();
};
