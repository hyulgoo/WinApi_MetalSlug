#include "pch.h"
#include "Game\04.Object\CObj.h"

#include "Game\02.Manager\05.EventMgr\CEventMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\03.Level\CLevel.h"

#include "Game\05.Component\Collider\CCollider.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\05.Component\AI\CAI.h"

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
	, m_eLevelLayer(LAYER::DEFAULT)
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
	, m_eLevelLayer(LAYER::DEFAULT)
{
	if (nullptr != _other.m_pAnimator)
	{
		m_pAnimator = _other.m_pAnimator->Clone();
		m_pAnimator->SetOwner(this);
	}
	if (nullptr != _other.m_pAnimator2)
	{
		m_pAnimator2 = _other.m_pAnimator2->Clone();
		m_pAnimator2->SetOwner(this);
	}
}

CObj::~CObj()
{
	delete m_pCollider;
	delete m_pAnimator;
	delete m_pAnimator2;
	delete m_pRigidbody;
	delete m_pPixelCollider;
	delete m_pAI;
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

Vec2 CObj::GetScreenPos() const
{
	const CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (nullptr != pLevel && pLevel->IsCameraLayer(m_eLevelLayer))
		return CCamera::GetInst()->GetRenderPos(m_vPos);
	return m_vPos;
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

	if (nullptr != m_pAI)
		m_pAI->final_tick();

	// 충돌체는 이번 프레임의 이동(입력, Rigidbody, AI 상태)이 모두 끝난 뒤 위치를 정한다.
	// 충돌 매니저가 그 위치로 검사한다 (예전에는 tick에서 정해 한 프레임 전 위치로 검사했다)
	if (nullptr != m_pCollider)
		m_pCollider->final_tick();
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


