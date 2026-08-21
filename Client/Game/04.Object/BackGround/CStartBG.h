#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

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

    virtual LAYER GetLayer() override { return LAYER::BACKGROUND; }

public:
    CLONE_DEACTIVATE(CStartBG);

public:
    CStartBG();
    virtual ~CStartBG() override;
};


