#include "pch.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\04.Object\CObj.h"

CCollider::CCollider(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_iOverlapCount(0)
	, m_bRender(false)
	, m_bScaleSet(false)
{
}

CCollider::CCollider(const CCollider& _other)
	: CComponent(nullptr)
	, m_vOffsetPos(_other.m_vOffsetPos)
	, m_vFinalPos(_other.m_vFinalPos)
	, m_vScale(_other.m_vScale)
	, m_iOverlapCount(0)
	, m_bRender(_other.m_bRender)
	, m_bScaleSet(_other.m_bScaleSet)
{
}

CCollider::~CCollider()
{
}

void CCollider::SetRender()
{
	m_bRender = !m_bRender;
}

void CCollider::tick()
{
	// 크기를 따로 정하지 않은 충돌체는 오너 크기를 따른다
	if (!m_bScaleSet)
		m_vScale = GetOwner()->GetScale();

	// 충돌체의 최종 위치값을 계산한다.
	m_vFinalPos = GetOwner()->GetPos() + m_vOffsetPos;

	// 겹침 카운트가 음수가 되면 안 됨
	assert( !(m_iOverlapCount < 0));
}

void CCollider::render(const HDC _dc)
{
	// 디버그 표시(Q)가 켜져 있을 때만 그린다. 렌더는 상태를 바꾸지 않는다
	// (예전에는 표시와 상관없이 매번 크기를 오너 크기로 덮어써 10x10 판정이 20x20이 됐다)
	if (!m_bRender)
		return;

	const HPEN   hPen       = CEngine::GetInst()->GetPen(0 < m_iOverlapCount ? PEN_TYPE::RED : PEN_TYPE::GREEN);
	const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

	// DC에 원래 있던 펜과 브러시를 저장해두고, 그리는 데 쓸 것들로 교체한다
	const HPEN   hOriginPen   = static_cast<HPEN>(SelectObject(_dc, hPen));
	const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));

	// 사각형 그리기 (충돌체 위치는 월드라 오너가 그려지는 화면 위치에 맞춘다)
	const Vec2 vScreenPos = GetOwner()->GetScreenPos() + m_vOffsetPos;
	Rectangle(_dc, static_cast<int>(vScreenPos.x - m_vScale.x / 2.f)
		, static_cast<int>(vScreenPos.y - m_vScale.y / 2.f)
		, static_cast<int>(vScreenPos.x + m_vScale.x / 2.f)
		, static_cast<int>(vScreenPos.y + m_vScale.y / 2.f));

	// DC의 GDI 오브젝트들을 원래 있던 것들로 되돌린다.
	SelectObject(_dc, hOriginPen);
	SelectObject(_dc, hOriginBrush);
}

void CCollider::BeginOverlap(CCollider* _pOther)
{
	++m_iOverlapCount;

	GetOwner()->BeginOverlap(_pOther);
}

void CCollider::OnOverlap(CCollider* _pOther)
{
	GetOwner()->OnOverlap(_pOther);
}

void CCollider::EndOverlap(CCollider* _pOther)
{
	--m_iOverlapCount;

	GetOwner()->EndOverlap(_pOther);
}
