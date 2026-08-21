#include "pch.h"
#include "CEditorPanel.h"
#include "CKeyMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CEditorPanel::CEditorPanel()
{
}

CEditorPanel::~CEditorPanel()
{
}

void CEditorPanel::tick()
{
	CUI::tick();
}

void CEditorPanel::render(HDC _dc)
{
	POINT ptResolution = CEngine::GetInst()->GetResolution();

	HBRUSH hNullBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);

	// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
	HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);

	Rectangle(_dc, ptResolution.x - ptResolution.x / 4, 0, ptResolution.x, ptResolution.y);

	// DC 의 GDI 오브젝트들을 기존의 펜과 브러시로 되돌린다.
	SelectObject(_dc, hOriginBrush);

	render_ChildUI(_dc);
}

void CEditorPanel::MouseLbtnDown()
{
	CUI::MouseLbtnDown();

	m_vPressPos = MOUSE_POS;
}

void CEditorPanel::MouseLbtnClicked()
{
	int a = 0;
}