#pragma once
#include "CObj.h"

class CTexture;

class CEditAnim :
    public CObj
{
private:
    wstring      m_AnimPath;
    Vec2         m_vStart;
    Vec2         m_vEnd;
    CTexture*    m_pAtlas;
    float        m_fmagni;

public:
    virtual void tick();
    virtual void render(HDC _dc);
    virtual LAYER GetLayer() { return LAYER::BACKGROUND; }
    float GetMagni() { return m_fmagni; }
    CLONE(CEditAnim);

public:
    CEditAnim();
    ~CEditAnim();
};

