#pragma once
#include "Game\04.Object\CObj.h"

class CSLDCQC :
    public CObj
{
private:
    bool  m_bStart;
    float m_fOffset;
    bool  m_bDir;
    float m_fTime;

public:
    virtual LAYER GetLayer() override { return LAYER::MONSTER_PROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void         SetDir(const bool _bRight) { m_bDir = _bRight; }


    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CSLDCQC();
    virtual ~CSLDCQC() override;
};
