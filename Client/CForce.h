#pragma once
#include "CObj.h"

class CForce :
    public CObj
{
private:
    float m_fAccTime;
    float m_fForce;     // ÈûÀÇ Å©±â
    float m_fRadius;    // ÈûÀÇ ¹Ý°æ
    float m_fLifeTime;

public:
    void SetLifeTime(float _fLifeTime) { m_fLifeTime = _fLifeTime; }
    void SetForceScale(float _fScale) { m_fForce = _fScale; }
    void SetForceRadius(float _fRadius) { m_fRadius = _fRadius; }

public:
    virtual void tick();
    virtual void render(HDC _dc);
    virtual LAYER GetLayer() { return LAYER::FORCE; }

public:
    CLONE(CForce);

public:
    CForce();
    ~CForce();
};

