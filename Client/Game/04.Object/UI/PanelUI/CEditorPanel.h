#pragma once
#include "Game\04.Object\UI\CUI.h"
class CEditorPanel :
    public CUI
{

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual void MouseLbtnClicked() override;

public:
    CLONE(CEditorPanel);

public:
    CEditorPanel();
    virtual ~CEditorPanel() override;
};

