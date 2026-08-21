#pragma once
#include "Game\04.Object\CObj.h"

class CCameraBox :
    public CObj
{
private:
    bool  m_bUpDown;
    float m_fDuration;
    float m_fMoveSpeed;

public:
    virtual void  tick() override;
    virtual void  render(HDC _dc) override;
    virtual LAYER GetLayer() override { return LAYER::EDITUI; }
    CLONE(CCameraBox)

    void SetMove(const bool _bUpDown, const float _fDuration, const float _fSpeed)
    {
        m_bUpDown    = _bUpDown;
        m_fDuration  = _fDuration;
        m_fMoveSpeed = _fSpeed;
    }

    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CCameraBox();
    virtual ~CCameraBox() override;
};
