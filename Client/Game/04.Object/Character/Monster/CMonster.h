#pragma once
#include "Game\04.Object\CObj.h"

class CPlayer;
class CTexture;

class CMonster :
    public CObj
{
private:
    UINT  m_iHP;
    CObj* m_pTarget;
    float m_fTime;
    int   m_iMaxCount;
    float m_fRatio;
    bool  m_bAttack;
    int   m_irand;

    tMonsterInfo m_tInfo;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    int          Getrand() const { return m_irand; }

public:
    CLONE(CMonster);

    tMonsterInfo GetMstInfo() const { return m_tInfo; }

    void SetMstSpeed(const float _tSpeed) { m_tInfo.m_fSpeed = _tSpeed; }
    void SetMstHP(const UINT _iHP) { m_tInfo.m_iHP = _iHP; }
    void SetMstAtkRange(const float _tRange) { m_tInfo.m_fAttackRange = _tRange; }
    void SetMstDtdRange(const float _tRange) { m_tInfo.m_fDetectRange = _tRange; }
    void SetMstAtkDelay(const float _tDelay) { m_tInfo.m_fAttackDelay = _tDelay; }
    void SetMstDeadTime(const float _fDeadTime) { m_tInfo.m_fDeadTime = _fDeadTime; }
    void SetTarget(CObj* _pPlayer) { m_pTarget = _pPlayer; }

    void SetMstInfo(float _fAttackRange, float _fAttackDelay, float _DetectRange, UINT _HP, float _fSpeed, float _fDeadTime);

    void SetAttack(const bool _bAttack) { m_bAttack = _bAttack; }
    bool IsAttack() const { return m_bAttack; }

public:
    CMonster();
    virtual ~CMonster() override;
};
