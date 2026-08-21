#pragma once
#include "Game\04.Object\CObj.h"

class CMouseArea :
    public CObj
{
private:
    Vec2  m_vStart;
    Vec2  m_vEnd;
    float m_fmagni;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void SetStartEnd(const Vec2& _vStart, const Vec2& _vEnd)
    {
        m_vStart = _vStart;
        m_vEnd   = _vEnd;
    }

    virtual LAYER GetLayer() override { return LAYER::EDITUI; }
    void          SetMagni(const float _fmagni) { m_fmagni = _fmagni; }

    CLONE(CMouseArea);

public:
    CMouseArea();
    virtual ~CMouseArea() override;
};
