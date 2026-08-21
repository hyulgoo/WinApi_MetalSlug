#include "pch.h"
#include "Game\04.Object\UI\PanelUI\CPanelUI.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

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
	{	// 이번 틱에 마우스가 이동한 만큼 Move를 계산해서 Pos에 더해줌
		const Vec2 vMove = MOUSE_POS - m_vPressPos;

		Vec2 vPos = GetPos();
		vPos += vMove;
		SetPos(vPos);

		m_vPressPos = MOUSE_POS;
	}

	CUI::tick();
}

void CPanelUI::render(const HDC _dc)
{
	if (nullptr == GetIdleTex())
		return;

	const Vec2 vPos = GetPos();

	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, static_cast<int>(GetIdleTex()->Width() * m_fmagni), static_cast<int>(GetIdleTex()->Height() * m_fmagni)
		, GetIdleTex()->GetDC()
		, 0
		, 0
		, static_cast<int>(GetIdleTex()->Width())		
		, static_cast<int>(GetIdleTex()->Height()), SRCCOPY);

	TransparentBlt(_dc
		, static_cast<int>(vPos.x)
		, static_cast<int>(vPos.y)
		, static_cast<int>(GetIdleTex()->Width() * m_fmagni)
		, static_cast<int>(GetIdleTex()->Height() * m_fmagni)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, static_cast<int>(GetIdleTex()->Width() * m_fmagni)
		, static_cast<int>(GetIdleTex()->Height() * m_fmagni)
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

