#pragma once
#include "CObj.h"
class CZB_PRJ_Effect :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bState;
    float       m_fDeadTime;
    float       m_fTime;
    bool        m_bStart;

    CTexture* m_pTexture;

public:
    LAYER GetLayer() { return LAYER::EFFECT; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void SetDir(bool _vLeft) { m_bDir = _vLeft; }
    void SetState(bool _vGroundHit) { m_bState = _vGroundHit; }

public:
    CZB_PRJ_Effect();
    ~CZB_PRJ_Effect();
};
