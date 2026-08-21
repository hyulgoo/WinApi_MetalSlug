#pragma once
#include "CObj.h"
class CTexture;

enum class STATE
{
    GROUND,
    HIT,
    WALL,
    BOMB,
    END
};

class CWeaponeEffect :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bStart;
    float       m_fDeadTime;
    float       m_fTime;
    STATE       m_tState;

    CTexture* m_pTexture;

public:
    LAYER GetLayer() { return LAYER::EFFECT; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void SetDir(bool _vLeft) { m_bDir = _vLeft; }
    void SetState(int _iGroundHit)  {
        if (_iGroundHit < 0 || _iGroundHit > 3)
            assert(false);
        m_tState = (STATE)_iGroundHit; }

public:
    CWeaponeEffect();
    ~CWeaponeEffect();
};