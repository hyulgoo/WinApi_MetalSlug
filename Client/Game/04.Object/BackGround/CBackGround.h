#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CBackGround :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pBackBuffer;
    Vec2      m_vResolution;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    CTexture*     GetBuffer() const { return m_pBackBuffer; }

public:
    CLONE_DEACTIVATE(CBackGround);

public:
    CBackGround();
    virtual ~CBackGround() override;
};
