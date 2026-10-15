#include "pch.h"
#include "Game\04.Object\Bullet\Player\CHandgun.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\04.Object\Bullet\Effect\CWeaponEffect.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

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
    GetRigidbody()->SetFallSpeed(0.f);   // 총알은 떨어지지 않는다
    SetScale(Vec2(1.f, 1.f));
    // 위치가 총알 그림의 중심이라 충돌체는 오프셋 없이 그 자리에 둔다
    // (예전에는 위치가 그림의 왼쪽 위라 충돌체만 (17, 10) 옮겨 두고, 착탄 이펙트는 왼쪽 위에서 터졌다)
    SetSpeed(1300.f);
    m_pTexture    = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
    m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BulletTexture", 50, 50);
}

CHandgun::~CHandgun()
{
}

void CHandgun::tick()
{
    Vec2        vPos    = GetPos();
    // 60분법 -> 라디안
    const float fRadian = (m_fDegree * PI) / 180.f;

    vPos.x += m_fSpeed * cosf(fRadian) * DT;
    vPos.y -= m_fSpeed * sinf(fRadian) * DT;

    SetPos(vPos);
    if (GetRigidbody()->IsGround())
    {
        CWeaponEffect* effect = new CWeaponEffect;
        effect->SetKind(CWeaponEffect::KIND::GROUND);
        Instantiate(effect, GetPos(), LAYER::EFFECT);
        if (!IsDead())
            SetDead();
    }
    else if (GetPixelCollider()->IsWall())
    {
        CWeaponEffect* effect = new CWeaponEffect;
        effect->SetKind(CWeaponEffect::KIND::WALL);
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

Vec2 CHandgun::GetSpriteSize() const
{
    // 아틀라스에서 자르는 크기 x 배율 (위·아래로 쏜 총알은 세운 그림)
    return m_bVRT ? Vec2(6.f * m_fmagni, 10.f * m_fmagni) : Vec2(10.f * m_fmagni, 6.f * m_fmagni);
}

void CHandgun::render(const HDC _dc)
{
    Vec2       vSize    = {};
    Vec2       vLeftTop = {};
    if (m_bVRT)
    {
        vSize    = Vec2(6.f, 10.f);
        vLeftTop = Vec2(78.f, 3.f);
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
    // 위치는 그림의 중심이다
    const Vec2 vPos = GetScreenPos() - GetSpriteSize() / 2.f;
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
        effect->SetKind(CWeaponEffect::KIND::HIT);
        Instantiate(effect, GetPos(), LAYER::EFFECT);
        if (!IsDead())
            SetDead();
    }
}
