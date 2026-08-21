#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CSLDBombEffect :
    public CObj
{
private:
    bool  m_bDir;
    bool  m_bState;
    float m_fDeadTime;
    float m_fTime;
    bool  m_bStart;

    CTexture* m_pTexture;

public:
    virtual LAYER GetLayer() override { return LAYER::EFFECT; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         SetState(const bool _vGroundHit) { m_bState = _vGroundHit; }

public:
    CSLDBombEffect();
    virtual ~CSLDBombEffect() override;
};
