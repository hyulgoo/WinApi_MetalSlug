#include "pch.h"
#include "CPanelUI.h"
#include "CKeyMgr.h"
#include "CTexture.h"
#include "CResMgr.h"

CPanelUI::CPanelUI()
	:m_pBackBuffer(nullptr)
	, m_fmagni(4.f)
{
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"UIBuffer", 300, 300);
}

CPanelUI::~CPanelUI()
{
}

void CPanelUI::tick()
{
	if (IsLbtnDown())
	{	// 이전 틱과 비교하여 차이나는 만큼 Move를 갱신하고 Pos에 더 해줌
		Vec2 vMove = MOUSE_POS - m_vPressPos;

		Vec2 vPos = GetPos();
		vPos += vMove;
		SetPos(vPos);

		m_vPressPos = MOUSE_POS;
	}

	CUI::tick();
}

void CPanelUI::render(HDC _dc)
{
	if (nullptr == GetIdleTex())
		return;

	Vec2 vPos = GetPos();

	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, int(GetIdleTex()->Width() * m_fmagni), int(GetIdleTex()->Height() * m_fmagni)
		, GetIdleTex()->GetDC()
		, 0
		, 0
		, (int)GetIdleTex()->Width()		
		, (int)GetIdleTex()->Height(), SRCCOPY);

	TransparentBlt(_dc
		, int(vPos.x)
		, int(vPos.y)
		, (int)(GetIdleTex()->Width() * m_fmagni)
		, (int)(GetIdleTex()->Height() * m_fmagni)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(GetIdleTex()->Width() * m_fmagni)
		, (int)(GetIdleTex()->Height() * m_fmagni)
		, RGB(255, 0, 255));

	render_ChildUI(_dc);
}

void CPanelUI::MouseLbtnDown()
{
	CUI::MouseLbtnDown();

	m_vPressPos = MOUSE_POS;
}

void CPanelUI::MouseLbtnClicked()
{
	int a = 0;
}

