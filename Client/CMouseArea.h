#pragma once
#include "CObj.h"
class CMouseArea :
    public CObj
{
private:
    Vec2    m_vStart;
    Vec2    m_vEnd;
    float   m_fmagni;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void SetStartEnd(Vec2 _vStart, Vec2 _vEnd)
    {
        m_vStart = _vStart;
        m_vEnd = _vEnd;
    }

    LAYER GetLayer() { return LAYER::EDITUI; }
    void SetMagni(float _fmagni) { m_fmagni = _fmagni; }

    CLONE(CMouseArea);

public:
    CMouseArea();
    ~CMouseArea();
};

