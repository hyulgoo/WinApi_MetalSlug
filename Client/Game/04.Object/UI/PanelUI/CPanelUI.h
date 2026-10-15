#pragma once
#include "Game\04.Object\UI\CUI.h"

class CTexture;

class CPanelUI :
    public CUI
{
private:
    Vec2 m_vPressPos;
    CTexture* m_pBackBuffer;
    float     m_fmagni;   // 그림 배율 (스테이지 HUD는 data\stage.ini [hud] magni)

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void SetMagni(const float _fMagni) { m_fmagni = _fMagni; }

    virtual void MouseLbtnDown() override;
    virtual void MouseLbtnClicked() override;
    
public:
    CLONE(CPanelUI);

public:
    CPanelUI();
    virtual ~CPanelUI() override;
};

