#include "pch.h"
#include "CPixelCollider.h"

#include "CCamera.h"
#include "CResMgr.h"
#include "CEngine.h"
#include "CTexture.h"
#include "CObj.h"
#include "CRigidbody.h"
#include "CTimeMgr.h"

CPixelCollider::CPixelCollider(CObj* _pOwner)
    : CComponent(_pOwner)
    , m_pTex(nullptr)
    , m_fmagni(4.f)
    , m_bPlayPixel(false)
    , m_bJump(false)
    , m_bGround(false)
    , m_bWall(false)
    , m_bGravity(true)
{
    m_vResolution = CEngine::GetInst()->GetResolution();
    m_OwnerPos    = GetOwner()->GetPos();
    m_vFinalPos   = m_OwnerPos + m_vOffsetPos;

    m_pAtlas = CResMgr::GetInst()->LoadTexture(L"PixelBackGroundasd", L"texture\\PixelBackGround.bmp");
    m_pTex   = CResMgr::GetInst()->CreateTexture(L"PixelBackGround_BackBuffer", static_cast<UINT>(m_vResolution.x + 400), static_cast<UINT>(m_vResolution.y + 400));
}

CPixelCollider::~CPixelCollider()
{
}

void CPixelCollider::tick()
{
    Vec2 vPos = Vec2(0.f, 0.f);
    vPos      = CCamera::GetInst()->GetRealPos(vPos);\
    //if (IsPressed(KEY::LEFT))
    //{
    //	vPos -= Vec2(300.f * DT, 0.f);
    //}
    //if (IsPressed(KEY::RIGHT))
    //{
    //	vPos += Vec2(300.f * DT, 0.f);
    //}
    if (m_bGravity)
        GetOwner()->SetPos(GetOwner()->GetPos() + Vec2(0.f, 300.f * DT));

    StretchBlt(m_pTex->GetDC()
             , 0, 0
             , static_cast<int>(m_vResolution.x + 400), static_cast<int>(m_vResolution.y + 200)
             , m_pAtlas->GetDC()
             , static_cast<int>(vPos.x)
             , static_cast<int>(vPos.y)
             , static_cast<int>((m_vResolution.x + 400) / m_fmagni), static_cast<int>((m_vResolution.y + 200) / m_fmagni)
             , SRCCOPY);


    m_OwnerScale    = GetOwner()->GetScale();
    m_OwnerPos      = GetOwner()->GetPos();
    m_vFinalPos     = m_OwnerPos + m_vOffsetPos;
    m_vGroundPoint1 = Vec2((m_vFinalPos.x), (m_vFinalPos.y + m_OwnerScale.y / 2));
    m_vGroundPoint2 = Vec2(m_vGroundPoint1.x, m_vGroundPoint1.y - 1);
    m_vWallPoint1   = Vec2((m_vFinalPos.x - m_OwnerScale.x / 2), (m_vFinalPos.y));
    m_vWallPoint2   = Vec2(m_vWallPoint1.x - 1, m_vWallPoint1.y);

    tColor           GroundCheck1 = m_pTex->GetPixel(static_cast<int>(m_vGroundPoint1.x), static_cast<int>(m_vGroundPoint1.y));
    tColor           GroundCheck2 = m_pTex->GetPixel(static_cast<int>(m_vGroundPoint2.x), static_cast<int>(m_vGroundPoint2.y));
    tColor           WallPoint1   = m_pTex->GetPixel(static_cast<int>(m_vWallPoint1.x), static_cast<int>(m_vWallPoint1.y));
    tColor           WallPoint2   = m_pTex->GetPixel(static_cast<int>(m_vWallPoint2.x), static_cast<int>(m_vWallPoint2.y));
    tColor           JumpPoint    = m_pTex->GetPixel(static_cast<int>(m_vGroundPoint1.x), static_cast<int>(m_vGroundPoint1.y) + 10);
    constexpr tColor magenta      = {255, 0, 255};
    constexpr tColor green        = {0, 255, 0};

    if (JumpPoint == magenta)
        m_bJump = false;
    else if (JumpPoint != magenta)
        m_bJump = true;

    if (WallPoint1 == green)
    {
        m_bWall   = true;
        m_bGround = false;
        // 벽에 파묻힌 상태를 벽 밖으로 밀어냄
        while (WallPoint2 == green)
        {
            // 바로 옆 픽셀을 체크해서, 벽 색상이 아닌 위치가 나올 때까지 오른쪽으로 밀어냄
            m_vWallPoint2.x += 1.f;
            GetOwner()->SetPos(Vec2(m_vWallPoint2.x + m_OwnerScale.x / 2 + 1, m_vWallPoint2.y));
            WallPoint2 = m_pTex->GetPixel(static_cast<int>(m_vWallPoint2.x), static_cast<int>(m_vWallPoint2.y));
        }
    }
    else if (GroundCheck1 == magenta)
    {
        // 땅에 파묻힌 상태를 땅 위로 밀어 올림
        m_bGround = true;
        m_bWall   = false;

        GetOwner()->GetRigidbody()->SetGround(true);
        while (GroundCheck2 == magenta)
        {
            // 바로 아래 픽셀을 체크해서, 땅 색상이 아닌 위치가 나올 때까지 위로 밀어 올림
            m_vGroundPoint2.y -= 1.f;
            GetOwner()->SetPos(Vec2(m_vGroundPoint2.x, (m_vGroundPoint2.y - m_OwnerScale.y / 2) + 1));
            GroundCheck2 = m_pTex->GetPixel(static_cast<int>(m_vGroundPoint2.x), static_cast<int>(m_vGroundPoint2.y));
        }
    }
    else
    {
        GetOwner()->SetPos(m_vFinalPos);
        GetOwner()->GetRigidbody()->SetGround(false);
        m_bGround = false;
        m_bWall   = false;
    }
}

void CPixelCollider::final_tick()
{
}

void CPixelCollider::render(const HDC _dc)
{
    if (m_bPlayPixel)
    {
        StretchBlt(_dc
                 , 0, 0
                 , static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
                 , m_pTex->GetDC()
                 , 0
                 , 0
                 , static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
                 , SRCCOPY);
    }

    //HPEN hPen = nullptr;
    //hPen = CEngine::GetInst()->GetPen(PEN_TYPE::WHITE);
    //HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    //
    //// DC에 원래 있던 펜과 브러시를 저장해두고, 그리는 데 쓸 것들로 교체한다
    //HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
    //HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);
    //
    //// 사각형 그리기
    //Rectangle(_dc, (int)(m_vGroundPoint2.x - 2)
    //	, (int)(m_vGroundPoint2.y -1)
    //	, (int)(m_vGroundPoint2.x + 2)
    //	, (int)(m_vGroundPoint2.y + 1));
    //
    //Rectangle(_dc, (int)(m_vWallPoint1.x - 1)
    //	, (int)(m_vWallPoint1.y - 2)
    //	, (int)(m_vWallPoint1.x + 1)
    //	, (int)(m_vWallPoint1.y + 2));
    //
    //// DC의 GDI 오브젝트들을 원래 있던 것들로 되돌린다
    //SelectObject(_dc, hOriginPen);
    //SelectObject(_dc, hOriginBrush);
}
