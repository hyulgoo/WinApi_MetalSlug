#pragma once
#include "CObj.h"

class CSLDCQC :
    public CObj
{
private:
    bool         m_bStart;
    float        m_fOffset;
    bool         m_bDir;
    float        m_fTime;

public:
    LAYER GetLayer() { return LAYER::MONSTER_PROJECTILE; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void SetDir(bool _bRight) {m_bDir = _bRight;}


    virtual void BeginOverlap(CCollider* _pOther) override;
public:
    CSLDCQC();
    ~CSLDCQC();
};

