#include "pch.h"
#include "CObj.h"

#include "CEventMgr.h"

#include "CCollider.h"
#include "CAnimator.h"
#include "CRigidbody.h"
#include "CPixelCollider.h"
#include "CAI.h"

CObj::CObj()
	: m_vPos{}
	, m_vScale{}	
	, m_pCollider(nullptr)
	, m_pAnimator(nullptr)
	, m_pAnimator2(nullptr)
	, m_pRigidbody(nullptr)
	, m_pPixelCollider(nullptr)
	, m_pAI(nullptr)
	, m_bDead(false)
{

}

CObj::CObj(const CObj& _other)
	: CEntity(_other)
	, m_vPos{ _other.m_vPos }
	, m_vScale{ _other.m_vScale }
	, m_pCollider(nullptr)
	, m_pAnimator(nullptr)
	, m_pAnimator2(nullptr)
	, m_pRigidbody(nullptr)
	, m_pPixelCollider(nullptr)
	, m_pAI(nullptr)
	, m_bDead(false)
{
	if (nullptr != _other.m_pAnimator)
	{
		m_pAnimator = _other.m_pAnimator->Clone();
		m_pAnimator->SetOwner(this);
	}
	if (nullptr != _other.m_pAnimator2)
	{
		m_pAnimator2 = _other.m_pAnimator->Clone();
		m_pAnimator2->SetOwner(this);
	}
}

CObj::~CObj()
{
	DEL(m_pCollider);
	DEL(m_pAnimator);
	DEL(m_pAnimator2);
	DEL(m_pRigidbody);
	DEL(m_pPixelCollider);
	DEL(m_pAI);
}


void CObj::tick()
{
	if (nullptr != m_pRigidbody)
		m_pRigidbody->tick();

	if (nullptr != m_pPixelCollider)
		m_pPixelCollider->tick();

	if (nullptr != m_pAnimator2)
		m_pAnimator2->tick();

	if (nullptr != m_pAnimator)
		m_pAnimator->tick();

	if (nullptr != m_pCollider)
		m_pCollider->tick();

	if (nullptr != m_pAI)
		m_pAI->tick();
}

void CObj::render(const HDC _dc)
{
	if (nullptr != m_pRigidbody)
		m_pRigidbody->render(_dc);

	if (nullptr != m_pPixelCollider)
		m_pPixelCollider->render(_dc);

	if (nullptr != m_pAnimator2)
		m_pAnimator2->render(_dc);

	if (nullptr != m_pAnimator)
		m_pAnimator->render(_dc);

	if (nullptr != m_pCollider)
		m_pCollider->render(_dc);

	if (nullptr != m_pAI)
		m_pAI->render(_dc);
}

void CObj::final_tick()
{
	if (nullptr != m_pRigidbody)
		m_pRigidbody->final_tick();

	if (nullptr != m_pPixelCollider)
		m_pPixelCollider->final_tick();

	if (nullptr != m_pAnimator2)
		m_pAnimator2->final_tick();

	if (nullptr != m_pAnimator)
		m_pAnimator->final_tick();

	if (nullptr != m_pCollider)
		m_pCollider->final_tick();

	if (nullptr != m_pAI)
		m_pAI->final_tick();
}



void CObj::CreateCollider()
{
	m_pCollider = new CCollider(this);
}

void CObj::CreateAnimator()
{
	m_pAnimator = new CAnimator(this);
}

void CObj::CreateAnimator2()
{
	m_pAnimator2 = new CAnimator(this);
}

void CObj::CreateRigidbody()
{
	m_pRigidbody = new CRigidbody(this);
}

void CObj::CreatePixelCollider()
{
	m_pPixelCollider = new CPixelCollider(this);
}

void CObj::CreateAI()
{
	m_pAI = new CAI(this);
}

void CObj::SetDead()
{
	tEvent evn = {};
	evn.eType = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)this;
	CEventMgr::GetInst()->AddEvent(evn);
}


