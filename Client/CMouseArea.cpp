#include "pch.h"
#include "CMouseArea.h"
#include "CEngine.h"
#include "CCamera.h"

CMouseArea::CMouseArea()
	: m_vStart{}
	, m_vEnd{}
	, m_fmagni(1)
{ 
}

CMouseArea::~CMouseArea()
{
}

void CMouseArea::tick()
{
	CObj::tick();
}

void CMouseArea::render(HDC _dc)
{
	HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN hPen = CEngine::GetInst()->GetPen(PEN_TYPE::WHITE);

	// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
	HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
	HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);
	Vec2 vPos{};
	vPos = CCamera::GetInst()->GetRenderPos(vPos);
	Rectangle(_dc
		, (int)(vPos.x + m_vStart.x * m_fmagni)
		, (int)(vPos.y + m_vStart.y * m_fmagni)
		, (int)(vPos.x + m_vEnd.x * m_fmagni)
		, (int)(vPos.y + m_vEnd.y * m_fmagni));

	SelectObject(_dc, hOriginPen);
	SelectObject(_dc, hOriginBrush);

	CObj::render(_dc);
}
