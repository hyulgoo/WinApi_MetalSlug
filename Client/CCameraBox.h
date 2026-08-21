#pragma once
#include "CObj.h"
class CCameraBox :
    public CObj
{
private:
    bool                m_bUpDown;
    float               m_fDuration;
    float               m_fMoveSpeed;

public:
    virtual void tick();
    virtual void render(HDC _dc);
    virtual LAYER GetLayer() { return LAYER::EDITUI; }
    CLONE(CCameraBox);
    void SetMove(bool _bUpDown, float _fDuration, float _fSpeed)
    {
        m_bUpDown = _bUpDown;
        m_fDuration = _fDuration;
        m_fMoveSpeed = _fSpeed;
    }
    virtual void BeginOverlap(CCollider* _pOther) override;
public:
    CCameraBox();
    ~CCameraBox();
};

