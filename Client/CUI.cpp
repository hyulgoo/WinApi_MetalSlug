#include "pch.h"
#include "CUI.h"

#include "CEngine.h"
#include "CKeyMgr.h"
#include "CTexture.h"

CUI::CUI()
	: m_bCmrAfcted(false)
	, m_bLbtnDown(false)
	, m_bMouseOn(false)
	, m_pIdleTex(nullptr)
	, m_pParentUI(nullptr)
{
}

CUI::CUI(const CUI& _other)
	:CObj(_other)
	, m_bCmrAfcted(_other.m_bCmrAfcted)
	, m_bLbtnDown(false)
	, m_bMouseOn(false)
	, m_pIdleTex(_other.m_pIdleTex)
	, m_pParentUI(nullptr)
{
	for (size_t i = 0; i < _other.m_vecChildUI.size(); ++i)
	{
		AddChildUI(_other.m_vecChildUI[i]->Clone());
	}
}

CUI::~CUI()
{
	for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		DEL(m_vecChildUI[i]);
}

void CUI::tick()
{
	m_vFinalPos = GetPos();

	if (m_pParentUI)
	{
		m_vFinalPos += m_pParentUI->GetFinalPos();
	}

	MouseOnCheck();

	for (size_t i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->tick();
	}
}

void CUI::render(HDC _dc)
{
	HPEN hPen = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);
	HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
	HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);

	Vec2 vPos = GetPos();
	if (m_bCmrAfcted)
	{
		vPos = CCamera::GetInst()->GetRenderPos(vPos);
	}

	Vec2 vScale = GetScale();

	Rectangle(_dc
		, (int)(vPos.x)
		, (int)(vPos.y)
		, (int)(vPos.x + vScale.x)
		, (int)(vPos.y + vScale.y));

	SelectObject(_dc, hOriginPen);
	SelectObject(_dc, hOriginBrush);

	render_ChildUI(_dc);
}

void CUI::render_ChildUI(HDC _dc)
{
	for (size_t i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->render(_dc);
	}
}

void CUI::SetIdleTex(CTexture* _pTex)
{
	m_pIdleTex = _pTex;

	if (nullptr != m_pIdleTex)
	{
		Vec2 vScale = Vec2((float)m_pIdleTex->Width(), (float)m_pIdleTex->Height());
		SetScale(vScale);
	}
}

void CUI::MouseOnCheck()
{
	Vec2 vPos = GetFinalPos();
	Vec2 vScale = GetScale();
	Vec2 vMousePos = MOUSE_POS;

	if (m_bCmrAfcted)
	{
		vMousePos = CCamera::GetInst()->GetRealPos(vMousePos);
	}

	if (vPos.x <= vMousePos.x && vMousePos.x <= vPos.x + vScale.x
		&& vPos.y <= vMousePos.y && vMousePos.y <= vPos.y + vScale.y)
	{
		m_bMouseOn = true;
	}

	else
	{
		m_bMouseOn = false;
	}
}

