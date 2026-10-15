#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

class CSelectAnim :
    public CObj
{
private:
    bool      m_bStart;
    int       m_iCurFrm;
    float     m_fmagni;
    bool      m_bPrev;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void SetStart() { m_bStart = true; }
    void SetNumber() { m_iCurFrm = 1; }

public:
    CSelectAnim();
    virtual ~CSelectAnim() override;

};

