#include "pch.h"
#include "Game\04.Object\UI\ButtonUI\CEditorButton.h"

#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CEditorButton::CEditorButton()
{
	const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"editor", L"button");
	m_tColor                  = tData.GetColor(L"color");
	m_tHoverColor             = tData.GetColor(L"hover_color");
	m_tPressColor             = tData.GetColor(L"press_color");
}

CEditorButton::~CEditorButton()
{
}

void CEditorButton::render(const HDC _dc)
{
	const Vec2 vPos   = GetFinalPos();
	const Vec2 vScale = GetScale();

	// 마우스가 올라가면 옅은 파랑, 누르고 있으면 조금 진하게 (DC 브러시는 선택해야 SetDCBrushColor가 먹는다)
	COLORREF tColor = m_tColor;
	if (IsMouseOn())
		tColor = IsLbtnDown() ? m_tPressColor : m_tHoverColor;
	const HBRUSH   hOldBrush = static_cast<HBRUSH>(SelectObject(_dc, GetStockObject(DC_BRUSH)));
	const COLORREF tOldColor = SetDCBrushColor(_dc, tColor);
	Rectangle(_dc
	        , static_cast<int>(vPos.x)
	        , static_cast<int>(vPos.y)
	        , static_cast<int>(vPos.x + vScale.x)
	        , static_cast<int>(vPos.y + vScale.y));
	SetDCBrushColor(_dc, tOldColor);
	SelectObject(_dc, hOldBrush);

	// 글자는 버튼 가운데. 정렬·배경 모드는 되돌린다 (예전에는 TA_CENTER를 남겨 뒤에 그리는 글자가 밀렸다)
	SIZE tTextSize = {};
	GetTextExtentPoint32(_dc, m_strText.c_str(), static_cast<int>(m_strText.size()), &tTextSize);
	const UINT iOldAlign = SetTextAlign(_dc, TA_CENTER);
	const int  iOldMode  = SetBkMode(_dc, TRANSPARENT);
	TextOut(_dc
	      , static_cast<int>(vPos.x + vScale.x / 2.f)
	      , static_cast<int>(vPos.y + (vScale.y - static_cast<float>(tTextSize.cy)) / 2.f)
	      , m_strText.c_str(), static_cast<int>(m_strText.size()));
	SetBkMode(_dc, iOldMode);
	SetTextAlign(_dc, iOldAlign);

	render_ChildUI(_dc);
}

void CEditorButton::MouseLbtnClicked()
{
	if (m_fnClick)
		m_fnClick();
}
