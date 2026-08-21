#pragma once
#include "Game\04.Object\UI\CUI.h"

class CTexture;

class CPanelUI :
    public CUI
{
private:
    Vec2 m_vPressPos;
    CTexture* m_pBackBuffer;
    float     m_fmagni;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual void MouseLbtnDown() override;
    virtual void MouseLbtnClicked() override;
    
public:
    CLONE(CPanelUI);

public:
    CPanelUI();
    virtual ~CPanelUI() override;
};

