#pragma once
#include "CObj.h"
class CTexture;

class CCharacter :
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

public:
    CLONE_DEACTIVATE(CCharacter)

public:
    CCharacter();
    virtual ~CCharacter() override;
};


