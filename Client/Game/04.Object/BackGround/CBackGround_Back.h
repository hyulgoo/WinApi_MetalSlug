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

    // 먼 배경: 크게 그리고, 카메라보다 천천히 움직이며(원근감), 텍스처 위쪽 일부는 쓰지 않는다 (data\stage.ini [background])
    float     m_fMagni;
    double    m_dParallax;
    int       m_iTop;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;


public:
    CLONE_DEACTIVATE(CBackGround_Back);

public:
    CBackGround_Back();
    virtual ~CBackGround_Back() override;
};
