#pragma once
#include "CEntity.h"

#include "CAI.h"

class CPlayer;
class CMonster;
class CLevel;

class CState :
    public CEntity
{
private:
    CAI*        m_pOwnerAI;
    bool        m_bCQC;
    Vec2        m_vMontoPly;

public:
    virtual void final_tick() = 0;
    virtual void Enter() = 0;
    virtual void Exit() = 0;

public:
    CAI*  GetOwnerAI() const { return m_pOwnerAI; }
    CObj* GetOwnerObj() const { return m_pOwnerAI->GetOwner(); }
    bool  GetCQC() const { return m_bCQC; }
    void  SetCQC() { m_bCQC = true; }
    void  ChangeState(const wchar_t* _strStateName) const;
    void  SetMontoPlayer(const Vec2& _vMtP) {m_vMontoPly = _vMtP; }
    Vec2  GetMontoPlayer() { return m_vMontoPly; }
     
public:
    virtual CState* Clone() override = 0;

public:
    CState();
    ~CState();

    friend class CAI;
};

