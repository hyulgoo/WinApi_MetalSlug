#pragma once
#include "CObj.h"

class CTexture;

class CUI :
    public CObj
{
private:
    vector<CUI*>    m_vecChildUI;
    CUI*            m_pParentUI;
    CTexture*       m_pIdleTex;
    Vec2            m_vFinalPos;

    bool            m_bMouseOn;
    bool            m_bLbtnDown;
    // 카메라의 영향을 받는지
    bool            m_bCmrAfcted;
    
public:
    virtual void tick();
    virtual void render(HDC _dc);

    void render_ChildUI(HDC _dc);

public:
    // UI의 카메라 영향 여부를 설정
    void SetCameraAffected(bool _bSet) { m_bCmrAfcted = _bSet; }
    void SetIdleTex(CTexture* _pTex);
    bool IsMouseOn() { return m_bMouseOn; }
    bool IsLbtnDown() { return m_bLbtnDown; }

public:
    virtual void MouseLbtnDown() { m_bLbtnDown = true; }
    virtual void MouseLbtnClicked() {};
    virtual void MouseOn() {};

    virtual LAYER GetLayer() { return LAYER::UI; }
    CTexture* GetIdleTex() { return m_pIdleTex; }
    Vec2 GetFinalPos() { return m_vFinalPos; }
    const vector<CUI*> GetChildUI() { return m_vecChildUI; }

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
    ~CUI();

    friend class CUIMgr;

    
};

