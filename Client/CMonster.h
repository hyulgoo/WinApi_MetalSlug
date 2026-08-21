#pragma once
#include "CObj.h"

class CPlayer;
class CTexture;

class CMonster :
    public CObj
{
private:
    UINT        m_iHP;
    CObj*       m_pTarget;
    float       m_fTime;
    int         m_iMaxCount;
    float       m_fRatio;
    bool        m_bAttack;
    int         m_irand;

    tMonsterInfo  m_tInfo;


public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    int Getrand() { return m_irand; }

public:
    CLONE(CMonster);

    tMonsterInfo GetMstInfo() { return m_tInfo; }

    void SetMstSpeed(float _tSpeed) { m_tInfo.m_fSpeed = _tSpeed; }
    void SetMstHP(UINT _iHP) { m_tInfo.m_iHP = _iHP; }
    void SetMstAtkRange(float _tRange) { m_tInfo.m_fAttackRange = _tRange; }
    void SetMstDtdRange(float _tRange) { m_tInfo.m_fDetectRange = _tRange; }
    void SetMstAtkDelay(float _tDelay) { m_tInfo.m_fAttackDelay = _tDelay; }
    void SetMstDeadTime(float _fDeadTime) {m_tInfo.m_fDeadTime = _fDeadTime;}
    void SetTarget(CObj* _pPlayer) { m_pTarget = _pPlayer; }

    void SetMstInfo(float _fAttackRange, float _fAttackDelay, float _DetectRange, UINT HP, float _fSpeed, float _fDeadTime);

    void SetAttack(bool _bAttack) { m_bAttack = _bAttack; }
    bool IsAttack() { return m_bAttack; }


public:
    CMonster();         
    ~CMonster();
};

