#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;
class CAnimator;

class CEditPlayer :
    public CObj
{
public:
    CTexture*   m_pAtlas;
    CAnimator*  m_pAnimator;
public:

    virtual void  tick() override;
    virtual void  render(HDC _dc) override;
    virtual LAYER GetLayer() override { return LAYER::EDITANIM; }

public:
    CLONE_DEACTIVATE(CEditPlayer);
    CTexture*  GetAtlas() const { return m_pAtlas; }
    CAnimator* GetAnimClone() const { return m_pAnimator; }

public:
    CEditPlayer();
    virtual ~CEditPlayer() override;
};

