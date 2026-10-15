#include "pch.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\CObj.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include <cmath>

namespace
{
    constexpr tColor GROUND = {255, 0, 255};   // 마젠타: 밟고 서는 땅
    constexpr tColor WALL   = {0, 255, 0};     // 초록: 지나갈 수 없는 벽

    // 충돌 맵에서 월드 위치의 색 (배경 텍스처 1px = 월드 BG_MAGNI). 맵 밖은 빈 곳이다
    tColor Sample(const CTexture* _pMap, const Vec2& _vWorld)
    {
        return _pMap->GetPixel(static_cast<int>(floorf(_vWorld.x / BG_MAGNI)), static_cast<int>(floorf(_vWorld.y / BG_MAGNI)));
    }

    const CTexture* CurrentCollisionMap()
    {
        const CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
        return nullptr == pLevel ? nullptr : pLevel->GetCollisionMap();
    }
}

CPixelCollider::CPixelCollider(CObj* _pOwner)
    : CComponent(_pOwner)
    , m_vOffsetPos{}
    , m_bShowMap(false)
    , m_bAirborne(false)
    , m_fGroundProbe(CDataMgr::GetInst()->GetSection(L"game", L"pixel_collider").GetFloat(L"ground_probe"))
    , m_bGround(false)
    , m_bWall(false)
{
}

CPixelCollider::~CPixelCollider()
{
}

void CPixelCollider::tick()
{
    const CTexture* pMap = CurrentCollisionMap();
    if (nullptr == pMap)
    {
        m_bAirborne   = true;
        m_bGround = false;
        m_bWall   = false;
        return;
    }

    CObj*       pOwner     = GetOwner();
    CRigidbody* pRigidbody = pOwner->GetRigidbody();
    const Vec2  vScale     = pOwner->GetScale();
    const Vec2  vPos       = pOwner->GetPos() + m_vOffsetPos;

    // 판정점: 발(아래 가운데), 왼쪽·오른쪽 벽(옆 가운데)
    const Vec2 vFoot  = Vec2(vPos.x, vPos.y + vScale.y / 2.f);
    const Vec2 vLeft  = Vec2(vPos.x - vScale.x / 2.f, vPos.y);
    const Vec2 vRight = Vec2(vPos.x + vScale.x / 2.f, vPos.y);

    m_bAirborne = Sample(pMap, vFoot + Vec2(0.f, m_fGroundProbe)) != GROUND;

    if (Sample(pMap, vLeft) == WALL)
    {
        // 왼쪽이 벽에 묻혔다: 벽이 끝나는 곳까지 오른쪽으로 밀어낸다
        m_bWall   = true;
        m_bGround = false;

        float fX     = vLeft.x - 1.f;
        bool  bMoved = false;
        while (Sample(pMap, Vec2(fX, vLeft.y)) == WALL)
        {
            fX     += 1.f;
            bMoved = true;
        }
        if (bMoved)
            pOwner->SetPos(Vec2(fX + vScale.x / 2.f + 1.f, vLeft.y) - m_vOffsetPos);
    }
    else if (Sample(pMap, vRight) == WALL)
    {
        // 오른쪽이 벽에 묻혔다: 왼쪽으로 밀어낸다 (예전에는 왼쪽 벽만 검사했다)
        m_bWall   = true;
        m_bGround = false;

        float fX     = vRight.x + 1.f;
        bool  bMoved = false;
        while (Sample(pMap, Vec2(fX, vRight.y)) == WALL)
        {
            fX     -= 1.f;
            bMoved = true;
        }
        if (bMoved)
            pOwner->SetPos(Vec2(fX - vScale.x / 2.f - 1.f, vRight.y) - m_vOffsetPos);
    }
    else if (Sample(pMap, vFoot) == GROUND)
    {
        // 발이 땅에 묻혔다: 땅 위로 밀어 올린다
        m_bGround = true;
        m_bWall   = false;
        if (nullptr != pRigidbody)
            pRigidbody->SetGround(true);

        float fY     = vFoot.y - 1.f;
        bool  bMoved = false;
        while (Sample(pMap, Vec2(vFoot.x, fY)) == GROUND)
        {
            fY     -= 1.f;
            bMoved = true;
        }
        if (bMoved)
            pOwner->SetPos(Vec2(vFoot.x, fY - vScale.y / 2.f + 1.f) - m_vOffsetPos);
    }
    else
    {
        m_bGround = false;
        m_bWall   = false;
        if (nullptr != pRigidbody)
            pRigidbody->SetGround(false);
    }
}

void CPixelCollider::final_tick()
{
}

void CPixelCollider::render(const HDC _dc)
{
    if (!m_bShowMap)
        return;

    // 디버그: 화면에 보이는 범위의 충돌 맵을 배경처럼 확대해 덮어 그린다
    const CTexture* pMap = CurrentCollisionMap();
    if (nullptr == pMap)
        return;

    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    const Vec2 vCamLT      = CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)) / BG_MAGNI;
    StretchBlt(_dc
             , 0, 0
             , static_cast<int>(vResolution.x), static_cast<int>(vResolution.y)
             , pMap->GetDC()
             , static_cast<int>(vCamLT.x), static_cast<int>(vCamLT.y)
             , static_cast<int>(vResolution.x / BG_MAGNI), static_cast<int>(vResolution.y / BG_MAGNI)
             , SRCCOPY);
}
