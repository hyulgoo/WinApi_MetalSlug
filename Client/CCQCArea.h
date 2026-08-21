#pragma once
#include "CObj.h"

class CPlayer;

class CCQCArea :
    public CObj
{
private:
    bool  m_bCQC;
    float m_fTime;

public:
    virtual LAYER GetLayer() override { return LAYER::CQCAREA; }
    bool          GetCQC() const { return m_bCQC; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void OnOverlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    CCQCArea();
    virtual ~CCQCArea() override;
};
