#include "pch.h"
#include "CHandgun.h"

#include "CCamera.h"
#include "CTimeMgr.h"
#include "CCollider.h"
#include "CPixelCollider.h"
#include "CRigidbody.h"
#include "CWeaponEffect.h"

#include "CResMgr.h"
#include "CTexture.h"

CHandgun::CHandgun()
    : m_fSpeed(200.f)
    , m_fDegree(80.f)
    , m_fTime(0.f)
    , m_pTexture(nullptr)
    , m_fmagni(3.3f)
{
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();
    GetRigidbody()->SetGravity(false);
    GetPixelCollider()->SetGravity();
    SetScale(Vec2(1.f, 1.f));
    GetCollider()->SetOffsetPos(Vec2(17.f, 10.f));
    SetSpeed(1300.f);
    m_pTexture    = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
    m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BulletTexture", 50, 50);
}

CHandgun::~CHandgun()
{
}

void CHandgun::tick()
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

    Vec2        vPos    = GetPos();
    // 60분법 -> 라디안
    const float fRadian = (m_fDegree * PI) / 180.f;

    vPos.x += m_fSpeed * cosf(fRadian) * DT;
    vPos.y -= m_fSpeed * sinf(fRadian) * DT;

    SetPos(vPos);
    if (GetRigidbody()->IsGround())
    {
        CWeaponEffect* effect = new CWeaponEffect;
        effect->SetState(0);
        Instantiate(effect, GetPos(), LAYER::EFFECT);
        if (!IsDead())
            SetDead();
    }
    else if (GetPixelCollider()->IsWall())
    {
        CWeaponEffect* effect = new CWeaponEffect;
        effect->SetState(2);
        Instantiate(effect, GetPos(), LAYER::EFFECT);
        if (!IsDead())
            SetDead();
    }
    // 일정 시간이 지나면 총알을 없앰
    m_fTime += DT;
    if (0.75f < m_fTime && !IsDead())
    {
        SetDead();
    }

    CObj::tick();
}

void CHandgun::render(const HDC _dc)
{
    const Vec2 vPos     = GetPos();
    Vec2       vSize    = {};
    Vec2       vLeftTop = {};
    if (m_bVRT)
    {
        vSize    = Vec2(6.f, 10.f);
        vLeftTop = Vec2(78.f, 3.f);
        GetCollider()->SetOffsetPos(Vec2(10.f, 10.f));
    }
    else
    {
        vSize    = Vec2(10.f, 6.f);
        vLeftTop = Vec2(4.f, 5.f);
    }
    StretchBlt(m_pBackBuffer->GetDC()
             , 0, 0
             , static_cast<int>(vSize.x * m_fmagni), static_cast<int>(vSize.y * m_fmagni)
             , m_pTexture->GetDC()
             , static_cast<int>(vLeftTop.x)
             , static_cast<int>(vLeftTop.y)
             , static_cast<int>(vSize.x)
             , static_cast<int>(vSize.y), SRCCOPY);
    TransparentBlt(_dc
                 , static_cast<int>(vPos.x)
                 , static_cast<int>(vPos.y)
                 , static_cast<int>(vSize.x * m_fmagni), static_cast<int>(vSize.y * m_fmagni)
                 , m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(vSize.x * m_fmagni), static_cast<int>(vSize.y * m_fmagni)
                 , RGB(255, 0, 255));

    CObj::render(_dc);
}

void CHandgun::BeginOverlap(CCollider* _pOther)
{
    if (_pOther->GetOwner()->GetLayer() == LAYER::MONSTER)
    {
        CWeaponEffect* effect = new CWeaponEffect;
        effect->SetState(1);
        Instantiate(effect, GetPos(), LAYER::EFFECT);
        if (!IsDead())
            SetDead();
    }
}
