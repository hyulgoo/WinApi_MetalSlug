#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CSLD_Grenade :
    public CObj
{
public:
    bool  m_bDir;
    bool  m_bStart;
    float m_fDeadTime;
    float m_fShotTime;
    bool  m_bBounce;
    Vec2  m_vPower;

    CTexture* m_pTexture;

public:
    virtual LAYER GetLayer() override { return LAYER::MONSTER_PROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         AddVelocity(bool _bDir);

public:
    CSLD_Grenade();
    virtual ~CSLD_Grenade() override;
};
