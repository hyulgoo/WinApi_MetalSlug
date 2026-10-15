#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CZB_Bullet.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CZB_Bullet::CZB_Bullet()
    : m_bDir(false)
    , m_bStart(true)
    , m_fRemoveLeft(0.f)
    , m_fRemoveRight(0.f)
    , m_fRemoveBottom(0.f)
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"zombie_vomit");
    m_vSpeed                  = tData.GetVec2(L"speed");

    const CDataSection& tArea = CDataMgr::GetInst()->GetSection(L"weapon", L"projectile");
    m_fRemoveLeft             = tArea.GetFloat(L"remove_left");
    m_fRemoveRight            = tArea.GetFloat(L"remove_right");
    m_fRemoveBottom           = tArea.GetFloat(L"remove_bottom");

    SetScale(tData.GetVec2(L"size"));
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();

    GetCollider()->SetScale(tData.GetVec2(L"collider_size"));
    GetAnimator()->LoadAnimation(L"Enemy//ZB_MAN_PROJECTILE.anim");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_GROUND.anim");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_HIT.anim");
}

CZB_Bullet::~CZB_Bullet()
{
}

void CZB_Bullet::tick()
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

    if (m_bStart)
    {
        GetAnimator()->Play(L"ZB_MAN_PROJECTILE.anim", false);
        if (m_bDir)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
        m_bStart = false;
    }

    // 땅에 닿으면 퍼지는 이펙트를 남긴다
    if (GetRigidbody()->IsGround() && !IsDead())
    {
        Instantiate(new CAnimEffect(L"Enemy//ZB_PROJECTILE_GROUND.anim", m_bDir), GetPos(), LAYER::EFFECT);
        SetDead();
    }
    CObj::tick();
}

void CZB_Bullet::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CZB_Bullet::BeginOverlap(CCollider* /*_pOther*/)
{
    // 플레이어(또는 플레이어 총알)에 맞으면 터지는 이펙트를 남긴다
    if (!IsDead())
    {
        Instantiate(new CAnimEffect(L"Enemy//ZB_PROJECTILE_HIT.anim", m_bDir), GetPos(), LAYER::EFFECT);
        SetDead();
    }
}

void CZB_Bullet::AddVelocityRight() const
{
    GetRigidbody()->AddVelocity(m_vSpeed);
}

void CZB_Bullet::AddVelocityLeft() const
{
    GetRigidbody()->AddVelocity(Vec2(-m_vSpeed.x, m_vSpeed.y));
}
