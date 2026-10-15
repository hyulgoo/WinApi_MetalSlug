#include "pch.h"
#include "Game\04.Object\Bullet\Player\CBomb.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\04.Object\Bullet\Effect\CWeaponEffect.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\01.Engine\CEngine.h"

CBomb::CBomb()
    : m_bDir(false)
    , m_bStart(true)
    , m_fBounceTime(0.f)
    , m_bBounce(false)
    , m_bAirborne(false)
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
    GetRigidbody()->SetFallSpeed(0.f);   // 수류탄은 중력 가속으로만 떨어진다
    GetCollider()->SetScale(Vec2(10.f, 10.f));
    GetAnimator()->LoadAnimation(L"Player//GRENADE.anim");
}

CBomb::~CBomb()
{
}

void CBomb::tick()
{
    // 땅에 닿지 못하고 픽셀 충돌을 검사하는 범위(화면 주변) 밖으로 나가면 레벨이 끝날 때까지 떨어지므로 지운다
    // (적 투사체 CZB_Bullet·CSLD_Grenade와 같은 기준)
    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    const Vec2 vScreenPos  = GetScreenPos();
    if (vScreenPos.x < -100.f || vScreenPos.x > vResolution.x + 400.f || vScreenPos.y > vResolution.y + 200.f)
    {
        if (!IsDead())
            SetDead();
        return;
    }

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

    // 원작처럼 처음 땅에 닿으면 한 번 튀고, 다시 닿을 때 터진다.
    // 예전에는 처음 닿고 0.3초 뒤에 터져 튀어 오른 공중(바닥 위 약 30px)에서 폭발했다
    if (GetRigidbody()->IsGround())
    {
        if (!m_bBounce)
        {
            // 튀는 속도는 던질 때 가로 속도의 절반. 예전에는 남은 속도에 더해서 튄 뒤가 던질 때보다 빨랐다(500 -> 약 735)
            m_bBounce = true;
            m_vPower  = Vec2(m_vPower.x / 2, m_vPower.y);
            GetRigidbody()->SetVelocity(Vec2(m_bDir ? m_vPower.x : -m_vPower.x, m_vPower.y));
            GetAnimator()->Play(L"GRENADE.anim", false);
            if (m_bDir)
                GetAnimator()->GetAnimation()->SetPlayRight();
            else
                GetAnimator()->GetAnimation()->SetPlayLeft();
        }
        else if (m_bAirborne)
        {
            Explode();
            return;
        }
    }
    else if (m_bBounce)
    {
        m_bAirborne = true;
    }

    // 튄 뒤 땅에 다시 닿지 못하는 곳(벽 틈 등)에 걸려도 터지게 한다. 정상이면 튄 뒤 약 0.4초에 내려앉는다
    if (m_bBounce)
    {
        m_fBounceTime += DT;
        if (m_fBounceTime > BOUNCE_TIMEOUT)
            Explode();
    }
}

void CBomb::Explode()
{
    if (IsDead())
        return;

    CWeaponEffect* pEffect = new CWeaponEffect;
    pEffect->SetKind(CWeaponEffect::KIND::BOMB);
    Instantiate(pEffect, GetPos(), LAYER::EFFECT);
    SetDead();
}

void CBomb::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CBomb::BeginOverlap(CCollider* /*_pOther*/)
{
    Explode();
}

void CBomb::AddVelocity(const bool _bDir)
{
    m_bDir = _bDir;
    if (_bDir)
        GetRigidbody()->AddVelocity(Vec2(m_vPower.x, m_vPower.y));
    else
        GetRigidbody()->AddVelocity(Vec2(-m_vPower.x, m_vPower.y));
}
