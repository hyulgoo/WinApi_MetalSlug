#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CZB_Bullet :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bStart;
    
public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         AddVelocityRight() const;
    void         AddVelocityLeft() const;

public:
    CZB_Bullet();
    virtual ~CZB_Bullet() override;
};


