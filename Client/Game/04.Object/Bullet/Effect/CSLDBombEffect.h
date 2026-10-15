#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CSLDBombEffect :
    public CObj
{
private:
    bool  m_bDir;
    bool  m_bState;
    bool  m_bStart;

    CTexture* m_pTexture;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         SetState(const bool _vGroundHit) { m_bState = _vGroundHit; }

public:
    CSLDBombEffect();
    virtual ~CSLDBombEffect() override;
};
