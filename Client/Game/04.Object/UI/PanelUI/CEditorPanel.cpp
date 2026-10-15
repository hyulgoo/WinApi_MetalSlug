#include "pch.h"
#include "Game\04.Object\UI\PanelUI\CEditorPanel.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"

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
	const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));

	// DC 에 원래 있던 브러시를 새로 만든 것으로 교체한다
	const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));

	// 에디터가 정한 패널 자리와 크기 (예전에는 화면 크기에서 폭 1/4을 따로 계산했다)
	const Vec2 vPos   = GetFinalPos();
	const Vec2 vScale = GetScale();
	Rectangle(_dc, static_cast<int>(vPos.x), static_cast<int>(vPos.y), static_cast<int>(vPos.x + vScale.x), static_cast<int>(vPos.y + vScale.y));

	// DC 의 GDI 오브젝트들을 원래 있던 브러시로 되돌린다.
	SelectObject(_dc, hOriginBrush);

	render_ChildUI(_dc);
}

void CEditorPanel::MouseLbtnClicked()
{
}