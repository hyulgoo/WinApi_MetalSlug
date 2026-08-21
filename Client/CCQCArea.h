#pragma once
#include "CObj.h"

class CPlayer;

class CCQCArea :
    public CObj
{
private:
    bool        m_bCQC;
    float       m_fTime;

public:
    LAYER GetLayer() { return LAYER::CQCAREA; }
    bool GetCQC() { return m_bCQC; }
public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void OnOverlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;


public:
    CCQCArea();
    ~CCQCArea();
};

