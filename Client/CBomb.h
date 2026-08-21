#pragma once
#include "CObj.h"
class CTexture;

class CBomb :
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
    virtual LAYER GetLayer() override { return LAYER::PLAYER_FIREPROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         AddVelocity(bool _bDir);

public:
    CBomb();
    virtual ~CBomb() override;
};
