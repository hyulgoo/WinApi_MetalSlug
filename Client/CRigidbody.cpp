#include "pch.h"
#include "CObj.h"

#include "CRigidbody.h"
#include "CTimeMgr.h"


CRigidbody::CRigidbody(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_fMass(1.f)
	, m_fFriction(400.f)
	, m_fFrictionScale(3.f)
	, m_fVelocityLimit(1000.f)
	, m_fGravityVLimit(1000.f)
	, m_bGravityUse(false)
	, m_fGravityAccel(500.f)
	, m_bGround(false)	
{
}

CRigidbody::CRigidbody(const CRigidbody& _other)
	: CComponent(nullptr)
	, m_fMass(_other.m_fMass)
	, m_fFriction(_other.m_fFriction)
	, m_fFrictionScale(_other.m_fFrictionScale)
	, m_fVelocityLimit(_other.m_fGravityVLimit)
	, m_fGravityVLimit(_other.m_fGravityVLimit)
	, m_bGravityUse(_other.m_bGravityUse)
	, m_fGravityAccel(_other.m_fGravityAccel)
	, m_bGround(false)
{
}

CRigidbody::~CRigidbody()
{
}

void CRigidbody::tick()
{
}

void CRigidbody::final_tick()
{
	// 가속도 = 힘 x 질량
	Vec2 vAccel = m_vForce * m_fMass;

	// 가속도를 가중시켜 속도를 증가시킴
	m_vVelocity += vAccel * DT;
	
	if (m_bGravityUse && m_bGround || !m_bGravityUse)
	{ //마찰 가속도
		Vec2 vFriction = -m_vVelocity;

		// 현재 마찰이 0이 아니라면
		if (!vFriction.IsZero())
		{
			// 마찰력 = 마찰력 * 마찰계수 * 질량 * 시간
			vFriction.Normalize();
			vFriction *= (m_fFriction * m_fFrictionScale * m_fMass * DT);
		}

		// 힘의 크기(길이)를 비교 후 마찰력의 크기가 더 길다면 힘을 0으로
		if (m_vVelocity.Length() < vFriction.Length())
		{
			m_vVelocity = Vec2(0.f, 0.f);
		}

		// 가속도에 마찰력을 더함
		m_vVelocity += vFriction;
	}
		// 공중상태이며 중력 적용이 true일 때 중력 적용
	if (m_bGravityUse && !m_bGround)
	{
		Vec2 vGravityAccel = Vec2(0.f, m_fGravityAccel);
		m_vVelocity += vGravityAccel * DT;
	}
	
	// 제한속도

	 // 중력이 작용 중일 때 x축은 일반적인 제한속도 y축은 중력제한속도
	if (m_bGravityUse)
	{
		if (m_fVelocityLimit < fabsf(m_vVelocity.x))
		{
			m_vVelocity.x = m_vVelocity.x / fabsf(m_vVelocity.x) * m_fVelocityLimit;
		}

		else if (m_fGravityVLimit < fabsf(m_vVelocity.y))
		{
			m_vVelocity.y = m_vVelocity.y / fabsf(m_vVelocity.y) * m_fGravityVLimit;
		}
	}
	 // 중력이 작용중이지 않으면 일반적인 제한속도 적용
	else
	{
		if (m_fVelocityLimit < m_vVelocity.Length())
		{
			m_vVelocity.Normalize();
			m_vVelocity *= m_fVelocityLimit;
		}
	}

	// 최종적으로 계산된 속도를 DT마다 적용시킴
	Vec2 vPos = GetOwner()->GetPos();

	vPos.x += m_vVelocity.x * DT;
	vPos.y += m_vVelocity.y * DT;

	GetOwner()->SetPos(vPos);

	m_vForce = Vec2(0.f, 0.f);
}

void CRigidbody::render(HDC _dc)
{
}

void CRigidbody::SetGround(bool _bGround)
{
	m_bGround = _bGround;

	if (m_bGround)
	{
		if (m_vVelocity.y > 0.f)
			m_vVelocity.y = 0;		
	}
}
