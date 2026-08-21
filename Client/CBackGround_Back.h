#pragma once
#include "CObj.h"

class CTexture;

class CBackGround_Back :
    public CObj
{
private:
    CTexture*  m_pAtlas;
    CTexture*  m_pBackBuffer;
    Vec2       m_vecResolution;
    float      m_fmagni;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual LAYER GetLayer() { return LAYER::BACKGROUND_BACK; }

public:
    CLONE_DEACTIVATE(CBackGround_Back);

public:
    CBackGround_Back();
    ~CBackGround_Back();
};

