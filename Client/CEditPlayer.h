#pragma once
#include "CObj.h"

class CTexture;
class CAnimator;

class CEditPlayer :
    public CObj
{
public:
    CTexture*   m_pAtlas;
    CAnimator*  m_pAnimator;
public:

    virtual void tick();
    virtual void render(HDC _dc);
    virtual LAYER GetLayer() { return LAYER::EDITANIM; }

public:
    CLONE_DEACTIVATE(CEditPlayer);
    CTexture* GetAtlas() { return m_pAtlas; }
    CAnimator* GetAnimClone() { return m_pAnimator; }

public:
    CEditPlayer();
    ~CEditPlayer();
};

