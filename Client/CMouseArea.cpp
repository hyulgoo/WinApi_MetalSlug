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

void CMouseArea::render(const HDC _dc)
{
    const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    const HPEN   hPen       = CEngine::GetInst()->GetPen(PEN_TYPE::WHITE);

    // DC에 원래 있던 펜과 브러시를 저장해두고, 그리는 데 쓸 것들로 교체한다
    const HPEN   hOriginPen   = static_cast<HPEN>(SelectObject(_dc, hPen));
    const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));
    Vec2         vPos{};
    vPos = CCamera::GetInst()->GetRenderPos(vPos);
    Rectangle(_dc
            , static_cast<int>(vPos.x + m_vStart.x * m_fmagni)
            , static_cast<int>(vPos.y + m_vStart.y * m_fmagni)
            , static_cast<int>(vPos.x + m_vEnd.x * m_fmagni)
            , static_cast<int>(vPos.y + m_vEnd.y * m_fmagni));

    SelectObject(_dc, hOriginPen);
    SelectObject(_dc, hOriginBrush);

    CObj::render(_dc);
}
