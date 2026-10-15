#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

class CStartBG :
    public CObj
{
private:
    CTexture* m_pAtlas;
    Vec2      m_vResolution;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;


public:
    CLONE_DEACTIVATE(CStartBG);

public:
    CStartBG();
    virtual ~CStartBG() override;
};


