#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CBackGround :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pBackBuffer;
    float     m_fmagni;
    Vec2      m_vResolution;
    Vec2      m_vDiff;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual LAYER GetLayer() override { return LAYER::BACKGROUND; }
    CTexture*     GetBuffer() const { return m_pBackBuffer; }

public:
    CLONE_DEACTIVATE(CBackGround);

public:
    CBackGround();
    virtual ~CBackGround() override;
};
