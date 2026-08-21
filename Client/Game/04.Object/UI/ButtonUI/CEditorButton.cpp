#include "pch.h"
#include "Game\04.Object\UI\ButtonUI\CEditorButton.h"
#include "Game\06.Resource\Texture\CTexture.h"

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

void CEditorButton::SetButtonTexOut(HDC& hdc, const int x, const int y, const LPCTSTR Text) const
{
	TextOut(hdc, x, y, Text, lstrlen(Text));
}
void CEditorButton::render(HDC _dc)

{
	SetTextAlign(_dc, TA_CENTER);
	const Vec2 vPos = GetFinalPos();
	const Vec2 vScale = GetScale();
	Rectangle(_dc, 
	static_cast<int>(vPos.x), 
	static_cast<int>(vPos.y), 
	static_cast<int>(vPos.x + vScale.x), 
	static_cast<int>(vPos.y + vScale.y));
	SetButtonTexOut(_dc
		, static_cast<int>(vPos.x + vScale.x / 2)
		, static_cast<int>(vPos.y + vScale.y / 4), m_strTex);
	render_ChildUI(_dc);
}

void CEditorButton::MouseLbtnClicked()
{
	if (m_pInst && m_pDelegateFunc)
		(m_pInst->*m_pDelegateFunc)();
}
