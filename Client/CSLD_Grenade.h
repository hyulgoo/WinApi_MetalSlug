#pragma once
#include "CObj.h"
class CSLD_Grenade :
    public CObj
{
public:
    bool        m_bDir;
    bool        m_bStart;
    float       m_fDeadTime;
    float       m_fShotTime;
    bool        m_bBounce;
    Vec2        m_vPower;

    CTexture* m_pTexture;

public:
    LAYER GetLayer() { return LAYER::MONSTER_PROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void SetDir(bool _vLeft) { m_bDir = _vLeft; }
    void AddVelocity(bool _bDir);


public:
    CSLD_Grenade();
    ~CSLD_Grenade();
};

