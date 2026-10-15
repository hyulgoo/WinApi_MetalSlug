#pragma once
#include "Game\04.Object\UI\CUI.h"

class CTexture;

class CNumberUI :
    public CUI
{
private:
    int         m_iCurFrm;
    CTexture*   m_pAtlas;
    CTexture*   m_pBackBuffer;
    Vec2        m_vLeftTop;
    Vec2        m_vSize;
    Vec2        m_fOffset;
    float       m_fMagni;           // 그림 배율 (data\stage.ini [hud] magni)
    bool        m_bNone;
    bool        m_bLife;
    bool        m_bTime;

    // 목숨 숫자 앞의 글자 그림 (아틀라스 안의 자리와 크기, 숫자에서 옆으로 떨어진 거리)
    Vec2        m_vLabelLeftTop;
    Vec2        m_vLabelSize;
    float       m_fLabelX;

public:
    virtual void render(HDC _dc) override;
    void         SetNumber(const int _Num) { m_iCurFrm = _Num; }
    void         SetNone() { m_bNone = true; }
    void         SetDone() { m_bNone = false; }
    void         SetPrintTime() { m_bTime = true; }
    bool         IsNone() const { return m_bNone; }
    void         SetMagni(const float _fMagni) { m_fMagni = _fMagni; }
    void         SetLifeLabel(const Vec2& _vLeftTop, const Vec2& _vSize, const float _fX)
    {
        m_bLife         = true;
        m_vLabelLeftTop = _vLeftTop;
        m_vLabelSize    = _vSize;
        m_fLabelX       = _fX;
    }

    CLONE(CNumberUI);

public:
    CNumberUI();
    virtual ~CNumberUI() override;
};

