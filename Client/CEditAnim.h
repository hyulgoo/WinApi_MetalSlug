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
    virtual void  tick() override;
    virtual void  render(HDC _dc) override;
    virtual LAYER GetLayer() override { return LAYER::BACKGROUND; }
    float         GetMagni() const { return m_fmagni; }
    CLONE(CEditAnim);

public:
    CEditAnim();
    virtual ~CEditAnim() override;
};

