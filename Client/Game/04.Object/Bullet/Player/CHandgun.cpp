#include "pch.h"
#include "Game\04.Object\Bullet\Player\CHandgun.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\04.Object\Bullet\Effect\CWeaponEffect.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

CHandgun::CHandgun()
    : m_fSpeed(0.f)
    , m_fDegree(0.f)
    , m_fTime(0.f)
    , m_fLifeTime(0.f)
    , m_pTexture(nullptr)
    , m_fmagni(0.f)
{
    // 수치 (data\weapon.ini [bullet]). 그림 배율은 애니메이션보다 조금 작게 그린다
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"bullet");
    m_fmagni                  = tData.GetFloat(L"draw_magni");
    m_fLifeTime               = tData.GetFloat(L"life_time");
    m_vSideLeftTop            = tData.GetVec2(L"side_left_top");
    m_vSideSize               = tData.GetVec2(L"side_size");
    m_vVertLeftTop            = tData.GetVec2(L"vert_left_top");
    m_vVertSize               = tData.GetVec2(L"vert_size");

    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();
    GetRigidbody()->SetGravity(false);
    GetRigidbody()->SetFallSpeed(0.f);   // 총알은 떨어지지 않는다
    SetScale(tData.GetVec2(L"size"));
    // 위치가 총알 그림의 중심이라 충돌체는 오프셋 없이 그 자리에 둔다
    // (예전에는 위치가 그림의 왼쪽 위라 충돌체만 (17, 10) 옮겨 두고, 착탄 이펙트는 왼쪽 위에서 터졌다)
    SetSpeed(tData.GetFloat(L"speed"));
    m_pTexture    = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
    m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BulletTexture", 50, 50);   // 그림 크기 x 배율을 담는 버퍼
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
    if (m_fLifeTime < m_fTime && !IsDead())
    {
        SetDead();
    }

    CObj::tick();
}

Vec2 CHandgun::GetSpriteSize() const
{
    // 아틀라스에서 자르는 크기 x 배율 (위·아래로 쏜 총알은 세운 그림)
    return (m_bVRT ? m_vVertSize : m_vSideSize) * m_fmagni;
}

void CHandgun::render(const HDC _dc)
{
    const Vec2 vSize    = m_bVRT ? m_vVertSize : m_vSideSize;
    const Vec2 vLeftTop = m_bVRT ? m_vVertLeftTop : m_vSideLeftTop;
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
