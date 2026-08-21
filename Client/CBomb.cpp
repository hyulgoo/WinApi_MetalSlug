#include "pch.h"
#include "CBomb.h"

#include "CTimeMgr.h"
#include "CCollider.h"

#include "CResMgr.h"
#include "CRigidbody.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CWeaponEffect.h"
#include "CPixelCollider.h"

CBomb::CBomb()
    : m_bDir(false)
    , m_bStart(true)
    , m_fDeadTime(0.f)
    , m_fShotTime()
    , m_bBounce(false)
    , m_vPower(Vec2(500.f, -300.f))

{
    SetScale(Vec2(20.f, 20.f));
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();

    GetRigidbody()->SetGravity(true);
    GetRigidbody()->SetGravityAccel(1500.f);
    GetRigidbody()->SetFrictionScale(3.f);
    GetRigidbody()->SetFriction(300.f);
    GetPixelCollider()->SetGravity();
    GetCollider()->SetScale(Vec2(10.f, 10.f));
    m_pTexture = CResMgr::GetInst()->FindTexture(L"Weapone");
    GetAnimator()->LoadAnimation(L"Player//GRENADE.anim");
}

CBomb::~CBomb()
{
}

void CBomb::tick()
{
    CObj::tick();

    if (m_bStart)
    {
        GetAnimator()->Play(L"GRENADE.anim", false);
        if (m_bDir)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
        AddVelocity(m_bDir);
        m_bStart = false;
    }

    if (GetRigidbody()->IsGround())
    {
        if (!m_bBounce)
        {
            m_vPower = Vec2(m_vPower.x / 2, m_vPower.y);
            AddVelocity(m_bDir);
            GetAnimator()->Play(L"GRENADE.anim", false);
            if (m_bDir)
                GetAnimator()->GetAnimation()->SetPlayRight();
            else
                GetAnimator()->GetAnimation()->SetPlayLeft();
        }
        m_bBounce = true;
    }

    if (m_bBounce)
    {
        m_fDeadTime += DT;

        if (m_fDeadTime > 0.3f)
        {
            CWeaponEffect* pEffect = new CWeaponEffect;
            pEffect->SetDir(m_bDir);
            pEffect->SetState(3);
            Instantiate(pEffect, GetPos(), LAYER::EFFECT);
            if (!IsDead())
                SetDead();
        }
    }
}

void CBomb::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CBomb::BeginOverlap(CCollider* _pOther)
{
    CWeaponEffect* pEffect = new CWeaponEffect;
    pEffect->SetDir(m_bDir);
    pEffect->SetState(3);
    Instantiate(pEffect, GetPos(), LAYER::EFFECT);
    if (!IsDead())
        SetDead();
}

void CBomb::AddVelocity(const bool _bDir)
{
    m_bDir = _bDir;
    if (_bDir)
        GetRigidbody()->AddVelocity(Vec2(m_vPower.x, m_vPower.y));
    else
        GetRigidbody()->AddVelocity(Vec2(-m_vPower.x, m_vPower.y));
}
