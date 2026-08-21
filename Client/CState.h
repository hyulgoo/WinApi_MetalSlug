#pragma once
#include "CEntity.h"

#include "CAI.h"
#include "CObj.h"

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
    CAI* GetOwnerAI() { return m_pOwnerAI; }
    CObj* GetOwnerObj() { return m_pOwnerAI->GetOwner(); }
    bool GetCQC() { return m_bCQC; }
    void SetCQC() { m_bCQC = true; }
    void ChangeState(const wchar_t* _strStateName);
    void SetMontoPlayer(Vec2 _vMtP) {m_vMontoPly = _vMtP; }
    Vec2 GetMontoPlayer() { return m_vMontoPly; }
     
public:
    CState* Clone() = 0;

public:
    CState();
    ~CState();

    friend class CAI;
};

