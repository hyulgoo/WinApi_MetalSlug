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
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         Explode();     // 폭발 이펙트를 남기고 사라진다 (땅이나 플레이어에 닿았다)
    void         AddVelocity(bool _bDir);

public:
    CSLD_Grenade();
    virtual ~CSLD_Grenade() override;
};
