#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

enum class STATE
{
    GROUND,
    HIT,
    WALL,
    BOMB,
    END
};

class CWeaponEffect :
    public CObj
{
private:
    bool  m_bDir;
    bool  m_bStart;
    float m_fDeadTime;
    float m_fTime;
    STATE m_tState;

    CTexture* m_pTexture;

public:
    virtual LAYER GetLayer() override { return LAYER::EFFECT; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }

    void SetState(int _iGroundHit)
    {
        if (_iGroundHit < 0 || _iGroundHit > 3)
            assert(false);
        m_tState = static_cast<STATE>(_iGroundHit);
    }

public:
    CWeaponEffect();
    virtual ~CWeaponEffect() override;
};
