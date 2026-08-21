#include "pch.h"
#include "Game\04.Object\CObj.h"

#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

CRigidbody::CRigidbody(CObj* _pOwner)
    : CComponent(_pOwner)
    , m_fMass(1.f)
    , m_fFriction(400.f)
    , m_fFrictionScale(3.f)
    , m_fVelocityLimit(1000.f)
    , m_fGravityAccel(500.f)
    , m_fGravityVLimit(1000.f)
    , m_bGravityUse(false)
    , m_bGround(false)
{
}

CRigidbody::CRigidbody(const CRigidbody& _other)
    : CComponent(nullptr)
    , m_fMass(_other.m_fMass)
    , m_fFriction(_other.m_fFriction)
    , m_fFrictionScale(_other.m_fFrictionScale)
    , m_fVelocityLimit(_other.m_fGravityVLimit)
    , m_fGravityAccel(_other.m_fGravityAccel)
    , m_fGravityVLimit(_other.m_fGravityVLimit)
    , m_bGravityUse(_other.m_bGravityUse)
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
    const Vec2 vAccel = m_vForce * m_fMass;

    // 가속도를 매 프레임 속도에 더해줌
    m_vVelocity += vAccel * DT;

    if (m_bGravityUse && m_bGround || !m_bGravityUse)
    {
        // 마찰 감속
        Vec2 vFriction = -m_vVelocity;

        // 마찰 방향이 0이 아니라면
        if (!vFriction.IsZero())
        {
            // 마찰력 = 방향 * 마찰계수 * 마찰배율 * 질량 * 시간
            vFriction.Normalize();
            vFriction *= (m_fFriction * m_fFrictionScale * m_fMass * DT);
        }

        // 현재 속도(크기)보다 마찰의 크기가 더 크다면 속도를 0으로
        if (m_vVelocity.Length() < vFriction.Length())
            m_vVelocity = Vec2(0.f, 0.f);

        // 속도에 마찰을 적용
        m_vVelocity += vFriction;
    }
    // 공중 상태이며 중력 사용이 true일 때 중력 적용
    if (m_bGravityUse && !m_bGround)
    {
        const Vec2 vGravityAccel = Vec2(0.f, m_fGravityAccel);
        m_vVelocity              += vGravityAccel * DT;
    }

    // 제한속도

    // 중력이 작용 중일 땐 x축은 일반 제한속도, y축은 중력 제한속도
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
    // 중력이 작용하지 않을 때는 일반 제한속도 적용
    else
    {
        if (m_fVelocityLimit < m_vVelocity.Length())
        {
            m_vVelocity.Normalize();
            m_vVelocity *= m_fVelocityLimit;
        }
    }

    // 최종적으로 구한 속도를 DT만큼 위치에 반영
    Vec2 vPos = GetOwner()->GetPos();

    vPos.x += m_vVelocity.x * DT;
    vPos.y += m_vVelocity.y * DT;

    GetOwner()->SetPos(vPos);

    m_vForce = Vec2(0.f, 0.f);
}

void CRigidbody::render(HDC _dc)
{
}

void CRigidbody::SetGround(const bool _bGround)
{
    m_bGround = _bGround;

    if (m_bGround)
    {
        if (m_vVelocity.y > 0.f)
            m_vVelocity.y = 0;
    }
}
