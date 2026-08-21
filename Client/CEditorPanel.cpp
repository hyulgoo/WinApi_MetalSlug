#include "pch.h"
#include "CEditorPanel.h"
#include "CKeyMgr.h"
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

void CEditorPanel::render(const HDC _dc)
{
	const POINT ptResolution = CEngine::GetInst()->GetResolution();

	const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));

	// DC 에 원래 있던 브러시를 새로 만든 것으로 교체한다
	const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));

	Rectangle(_dc, ptResolution.x - ptResolution.x / 4, 0, ptResolution.x, ptResolution.y);

	// DC 의 GDI 오브젝트들을 원래 있던 브러시로 되돌린다.
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