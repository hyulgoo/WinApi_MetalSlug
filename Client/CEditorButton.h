#pragma once
#include "CUI.h"

typedef void(CEntity::* DELEGATE)(void);


class CEditorButton :
    public CUI
{
    private:
        CEntity*    m_pInst;
        DELEGATE    m_pDelegateFunc;

        CTexture*   m_pPressedTex;
        CTexture*   m_pHighlightTex;

        LPCTSTR    m_strTex;

    public:
        void SetDelegate(CEntity* _pInst, DELEGATE _Func)
        {
            m_pInst = _pInst;
            m_pDelegateFunc = _Func;
        }
        void SetPressedTex(CTexture* _pTex) { m_pPressedTex = _pTex; }
        void SetHighlightTex(CTexture* _pTex) { m_pHighlightTex = _pTex; }
        void SetButtonTexOut(HDC& hdc, int x, int y, LPCTSTR Text);
        void SetButtonTex(LPCTSTR _Tex) { m_strTex = _Tex; }


        virtual  void render(HDC _dc);

        CLONE(CEditorButton);

    private:
        virtual void MouseLbtnClicked();

    public:
        CEditorButton();
        ~CEditorButton();
};

