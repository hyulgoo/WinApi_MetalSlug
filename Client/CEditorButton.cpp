#include "pch.h"
#include "CEditorButton.h"
#include "CTexture.h"

CEditorButton::CEditorButton()
	: m_pInst(nullptr)
	, m_pDelegateFunc(nullptr)
	, m_pPressedTex(nullptr)
	, m_pHighlightTex(nullptr)
	, m_strTex()
{
}

CEditorButton::~CEditorButton()
{

}

void CEditorButton::SetButtonTexOut(HDC& hdc, int x, int y, LPCTSTR Text)
{
	TextOut(hdc, x, y, Text, lstrlen(Text));
}
void CEditorButton::render(HDC _dc)

{
	SetTextAlign(_dc, TA_CENTER);
	Vec2 vPos = GetFinalPos();
	Vec2 vScale = GetScale();
	Rectangle(_dc, 
	(int)(	vPos.x), 
	(int)(	vPos.y), 
	(int)(	vPos.x + vScale.x), 
	(int)(	vPos.y + vScale.y));
	SetButtonTexOut(_dc
		, (int)(vPos.x + vScale.x / 2)
		, (int)(vPos.y + vScale.y / 4), m_strTex);
	render_ChildUI(_dc);
}

void CEditorButton::MouseLbtnClicked()
{
	if (m_pInst && m_pDelegateFunc)
		(m_pInst->*m_pDelegateFunc)();
}
