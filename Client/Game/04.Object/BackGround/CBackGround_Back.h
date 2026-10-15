#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CBackGround_Back :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pBackBuffer;
    Vec2      m_vecResolution;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;


public:
    CLONE_DEACTIVATE(CBackGround_Back);

public:
    CBackGround_Back();
    virtual ~CBackGround_Back() override;
};
