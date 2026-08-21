#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CBackGround_Front :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pBackBuffer;
    Vec2      m_vecResolution;
    float     m_fmagni;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual LAYER GetLayer() override { return LAYER::BACKGROUND_FRONT; }

public:
    CLONE(CBackGround_Front);

public:
    CBackGround_Front();
    CBackGround_Front(const CBackGround_Front& _Other);
    virtual ~CBackGround_Front() override;
};
