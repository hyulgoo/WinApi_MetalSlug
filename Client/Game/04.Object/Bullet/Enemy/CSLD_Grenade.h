#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CSLD_Grenade :
    public CObj
{
private:
    bool  m_bDir;
    bool  m_bStart;
    Vec2  m_vPower;          // 던질 때 속도 (오른쪽으로 던질 때). data\weapon.ini [soldier_grenade]

    // 화면 밖으로 이만큼 나가면 지운다 (data\weapon.ini [projectile])
    float m_fRemoveLeft;
    float m_fRemoveRight;
    float m_fRemoveBottom;

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
