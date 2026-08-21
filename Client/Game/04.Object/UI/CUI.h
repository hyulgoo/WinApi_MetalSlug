#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CUI :
    public CObj
{
private:
    vector<CUI*> m_vecChildUI;
    CUI*         m_pParentUI;
    CTexture*    m_pIdleTex;
    Vec2         m_vFinalPos;

    bool m_bMouseOn;
    bool m_bLbtnDown;
    // 카메라의 영향을 받는지 여부
    bool m_bCmrAfcted;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void render_ChildUI(HDC _dc) const;

public:
    // UI가 카메라 영향을 받을지 여부를 설정
    void SetCameraAffected(const bool _bSet) { m_bCmrAfcted = _bSet; }
    void SetIdleTex(CTexture* _pTex);
    bool IsMouseOn() const { return m_bMouseOn; }
    bool IsLbtnDown() const { return m_bLbtnDown; }

public:
    virtual void MouseLbtnDown() { m_bLbtnDown = true; }

    virtual void MouseLbtnClicked()
    {
    }

    virtual void MouseOn()
    {
    }

    virtual LAYER       GetLayer() override { return LAYER::UI; }
    CTexture*           GetIdleTex() const { return m_pIdleTex; }
    Vec2                GetFinalPos() { return m_vFinalPos; }
    const vector<CUI*>& GetChildUI() { return m_vecChildUI; }

    void AddChildUI(CUI* _pChildUI)
    {
        _pChildUI->m_pParentUI = this;
        m_vecChildUI.push_back(_pChildUI);
    }

private:
    void MouseOnCheck();

public:
    CLONE(CUI);

public:
    CUI();
    CUI(const CUI& _other);
    virtual ~CUI() override;

    friend class CUIMgr;
};
