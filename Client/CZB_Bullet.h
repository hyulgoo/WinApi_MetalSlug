#pragma once
#include "CObj.h"

class CZB_Bullet :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bDead;
    bool        m_bStart;
    float       m_fDeadTime;
    float       m_fTime;
    float       m_fShotTime;

    CTexture* m_pTexture;
    
public:
    LAYER GetLayer() { return LAYER::MONSTER_PROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void SetDir(bool _vLeft) { m_bDir = _vLeft; }
    void AddVelocityRight();
    void AddVelocituLeft();


public:
    CZB_Bullet();
    ~CZB_Bullet();
};


