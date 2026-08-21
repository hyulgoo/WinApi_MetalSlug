#pragma once
#include "CUI.h"

class CTexture;

class CButton :
    public CUI
{
private:
    int         m_iCurFrm;
    CTexture*   m_pAtlas;
    CTexture*   m_pBackBuffer;
    Vec2        m_vLeftTop;
    Vec2        m_vSize;
    Vec2        m_fOffset;
    bool        m_bNone;
    int         m_iNumber;
    bool        m_bLife;
    bool        m_bTime;

public:
    virtual void render(HDC _dc) override;
    void         SetNumber(const int _Num) { m_iCurFrm = _Num; }
    void         SetNone() { m_bNone = true; }
    void         SetDone() { m_bNone = false; }
    void         SetPrintTime() { m_bTime = true; }
    bool         IsNone() const { return m_bNone; }
    void         SetPrintLife() { m_bLife = true; }

    CLONE(CButton);

public:
    CButton();
    virtual ~CButton() override;
};

