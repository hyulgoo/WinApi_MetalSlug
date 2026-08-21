#include "pch.h"
#include "CCollider.h"

#include "CEngine.h"
#include "CObj.h"
#include "CCamera.h"

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
	m_bRender = m_bRender ? false : true;
}

void CCollider::tick()
{
	// 충돌체의 최종 위치값을 결정한다.
	m_vFinalPos = GetOwner()->GetPos() + m_vOffsetPos;

	// 중첩수가 음수인 경우
	assert( !(m_iOverlapCount < 0));
}

void CCollider::render(HDC _dc)
{
	// 충돌체를 그린다.
	// 필요한 펜과 브러시를 만든다(또는 가져온다)	
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
	HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
	HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
	HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);
	m_vScale = GetOwner()->GetScale();

	// 사각형 그리기
	Rectangle(_dc, (int)(m_vFinalPos.x - m_vScale.x / 2.f)
		, (int)(m_vFinalPos.y - m_vScale.y / 2.f)
		, (int)(m_vFinalPos.x + m_vScale.x / 2.f)
		, (int)(m_vFinalPos.y + m_vScale.y / 2.f));

	// DC 의 GDI 오브젝트들을 기존의 펜과 브러시로 되돌린다.
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
