#pragma once
#include "CObj.h"
class CStartBG :
    public CObj
{
private:
    CTexture* m_pAtlas;
    float     m_fmagni;
    Vec2      m_vResolution;
    Vec2      m_vDiff;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual LAYER GetLayer() { return LAYER::BACKGROUND; }

public:
    CLONE_DEACTIVATE(CStartBG);

public:
    CStartBG();
    ~CStartBG();
};


