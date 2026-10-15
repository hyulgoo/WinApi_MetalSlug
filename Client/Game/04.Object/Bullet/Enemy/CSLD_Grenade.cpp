#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CSLD_Grenade.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CSLD_Grenade::CSLD_Grenade()
    : m_bDir(false)
    , m_bStart(true)
    , m_fRemoveLeft(0.f)
    , m_fRemoveRight(0.f)
    , m_fRemoveBottom(0.f)
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"soldier_grenade");
    m_vPower                  = tData.GetVec2(L"throw_speed");

    const CDataSection& tArea = CDataMgr::GetInst()->GetSection(L"weapon", L"projectile");
    m_fRemoveLeft             = tArea.GetFloat(L"remove_left");
    m_fRemoveRight            = tArea.GetFloat(L"remove_right");
    m_fRemoveBottom           = tArea.GetFloat(L"remove_bottom");

    SetScale(tData.GetVec2(L"size"));
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();

    GetRigidbody()->SetGravity(true);
    GetRigidbody()->SetGravityAccel(tData.GetFloat(L"gravity"));
    GetRigidbody()->SetFrictionScale(tData.GetFloat(L"friction_scale"));
    GetRigidbody()->SetFriction(tData.GetFloat(L"friction"));
    GetRigidbody()->SetFallSpeed(0.f);   // 수류탄은 중력 가속으로만 떨어진다
    GetCollider()->SetScale(tData.GetVec2(L"collider_size"));
    GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_BOMB_PROJECTILE.anim");
}

CSLD_Grenade::~CSLD_Grenade()
{
}

void CSLD_Grenade::tick()
{
    // 픽셀 충돌을 검사하는 범위(화면 주변) 밖으로 나가면 땅에 닿지 못하고 계속 떨어지므로 제거한다.
    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    const Vec2 vScreenPos  = GetScreenPos();
    if (vScreenPos.x < m_fRemoveLeft || vScreenPos.x > vResolution.x + m_fRemoveRight || vScreenPos.y > vResolution.y + m_fRemoveBottom)
    {
        if (!IsDead())
            SetDead();
        return;
    }

    CObj::tick();

    if (m_bStart)
    {
        GetAnimator()->Play(L"SOLDIER_BOMB_PROJECTILE.anim", true);
        if (m_bDir)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
        AddVelocity(m_bDir);
        m_bStart = false;
    }

    if (GetRigidbody()->IsGround())
        Explode();
}

void CSLD_Grenade::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CSLD_Grenade::BeginOverlap(CCollider* /*_pOther*/)
{
    Explode();
}

void CSLD_Grenade::Explode()
{
    if (IsDead())
        return;
    Instantiate(new CAnimEffect(L"Enemy//SOLDIER_BOMB_SFX.anim", m_bDir), GetPos(), LAYER::EFFECT);
    SetDead();
}

void CSLD_Grenade::AddVelocity(const bool _bDir)
{
    m_bDir = _bDir;
    if (_bDir)
        GetRigidbody()->AddVelocity(Vec2(m_vPower.x, m_vPower.y));
    else
        GetRigidbody()->AddVelocity(Vec2(-m_vPower.x, m_vPower.y));
}
