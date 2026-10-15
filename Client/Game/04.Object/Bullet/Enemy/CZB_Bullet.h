#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CZB_Bullet :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bStart;
    Vec2        m_vSpeed;          // 뱉을 때 속도 (오른쪽으로). data\weapon.ini [zombie_vomit]

    // 화면 밖으로 이만큼 나가면 지운다 (data\weapon.ini [projectile])
    float       m_fRemoveLeft;
    float       m_fRemoveRight;
    float       m_fRemoveBottom;
    
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


