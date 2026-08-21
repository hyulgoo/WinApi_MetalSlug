#include "pch.h"
#include "CCollider.h"

#include "CEngine.h"
#include "CObj.h"

CCollider::CCollider(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_iOverlapCount(0)
	, m_bRender(false)
{
}

CCollider::CCollider(const CCollider& _other)
	: CComponent(nullptr)
	, m_vOffsetPos(_other.m_vOffsetPos)
	, m_vFinalPos(_other.m_vScale)
	, m_iOverlapCount(0)
	, m_bRender(_other.m_bRender)
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
	// 충돌체의 최종 위치값을 계산한다.
	m_vFinalPos = GetOwner()->GetPos() + m_vOffsetPos;

	// 겹침 카운트가 음수가 되면 안 됨
	assert( !(m_iOverlapCount < 0));
}

void CCollider::render(const HDC _dc)
{
	// 충돌체를 그린다.
	// 필요한 경우 펜을 만든다(또는 가져온다)
	HPEN hPen = nullptr;
	if (m_bRender)
	{
		if (0 < m_iOverlapCount)
			hPen = CEngine::GetInst()->GetPen(PEN_TYPE::RED);
		else
			hPen = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);
	}
	else
	{
		hPen = CEngine::GetInst()->GetPen(PEN_TYPE::NONE);
	}
	const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

	// DC에 원래 있던 펜과 브러시를 저장해두고, 그리는 데 쓸 것들로 교체한다
	const HPEN   hOriginPen   = static_cast<HPEN>(SelectObject(_dc, hPen));
	const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));
	m_vScale            = GetOwner()->GetScale();

	// 사각형 그리기
	Rectangle(_dc, static_cast<int>(m_vFinalPos.x - m_vScale.x / 2.f)
		, static_cast<int>(m_vFinalPos.y - m_vScale.y / 2.f)
		, static_cast<int>(m_vFinalPos.x + m_vScale.x / 2.f)
		, static_cast<int>(m_vFinalPos.y + m_vScale.y / 2.f));

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
