#include "pch.h"
#include "CZB_Bullet.h"

#include "CTimeMgr.h"
#include "CCollider.h"

#include "CResMgr.h"
#include "CRigidbody.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CCamera.h"
#include "CZB_PRJ_Effect.h"


CZB_Bullet::CZB_Bullet()
    : m_bDir(false)
    , m_bDead(false)
    , m_bStart(true)
    , m_fDeadTime()
    , m_fTime()
    , m_fShotTime()

{
    SetScale(Vec2(20.f, 20.f));
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();

    GetCollider()->SetScale(Vec2(10.f, 10.f));
    m_pTexture = CResMgr::GetInst()->FindTexture(L"Zombie");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_MAN_PROJECTILE.anim");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_GROUND.anim");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_HIT.anim");
}

CZB_Bullet::~CZB_Bullet()
{
}

void CZB_Bullet::tick()
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

    if (m_bStart)
    {
        GetAnimator()->Play(L"ZB_MAN_PROJECTILE.anim", false);
        if (m_bDir)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
        m_bStart = false;
    }

    if (m_bDead == false)
    {
        if (GetRigidbody()->IsGround())
        {
            CZB_PRJ_Effect* pEffect = new CZB_PRJ_Effect;
            pEffect->SetDir(m_bDir);
            pEffect->SetState(true);
            Instantiate(pEffect, GetPos(), LAYER::EFFECT);
            if (!IsDead())
                SetDead();
        }
    }
    CObj::tick();
}

void CZB_Bullet::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CZB_Bullet::BeginOverlap(CCollider* _pOther)
{
    if (m_bDead == false)
    {
        CZB_PRJ_Effect* pEffect = new CZB_PRJ_Effect;
        pEffect->SetDir(m_bDir);
        pEffect->SetState(false);
        Instantiate(pEffect, GetPos(), LAYER::EFFECT);
        SetDead();
    }
}

void CZB_Bullet::AddVelocityRight() const
{
    GetRigidbody()->AddVelocity(Vec2(700.f, -200.f));
}

void CZB_Bullet::AddVelocityLeft() const
{
    GetRigidbody()->AddVelocity(Vec2(-700.f, -200.f));
}
