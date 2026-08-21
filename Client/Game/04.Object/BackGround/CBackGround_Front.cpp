#include "pch.h"
#include "Game\04.Object\BackGround\CBackGround_Front.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\01.Engine\CEngine.h"

CBackGround_Front::CBackGround_Front()
    : m_pAtlas(nullptr)
    , m_pBackBuffer(nullptr)
    , m_vecResolution{}
    , m_fmagni(4.f)
{
    m_vecResolution = CEngine::GetInst()->GetResolution();
    m_pAtlas        = CResMgr::GetInst()->LoadTexture(L"BackGround_Front", L"texture\\BackGround_Front01.bmp");
    m_pBackBuffer   = CResMgr::GetInst()->CreateTexture(L"BackGround_FrontBackBuffer", static_cast<UINT>(m_pAtlas->Width() * m_fmagni), static_cast<UINT>(m_pAtlas->Height() * m_fmagni));
    SetPos(Vec2(3658.f, 508));
}

CBackGround_Front::CBackGround_Front(const CBackGround_Front& _Other)
    : m_pAtlas(nullptr)
    , m_pBackBuffer(nullptr)
    , m_vecResolution(_Other.m_vecResolution)
    , m_fmagni(_Other.m_fmagni)
{
    m_pAtlas      = CResMgr::GetInst()->LoadTexture(L"BackGround_Front2", L"texture\\BackGround_Front02.bmp");
    m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_FrontBackBuffer2", static_cast<UINT>(m_pAtlas->Width() * m_fmagni), static_cast<UINT>(m_pAtlas->Height() * m_fmagni));
    SetPos(Vec2(5815.f, 322));
}

CBackGround_Front::~CBackGround_Front()
{
}

void CBackGround_Front::tick()
{
    CObj::tick();
}

void CBackGround_Front::render(const HDC _dc)
{
    const Vec2 vPos    = GetPos();
    const Vec2 vDifPos = CCamera::GetInst()->GetRenderPos(Vec2(0.f, 0.f));

    StretchBlt(m_pBackBuffer->GetDC()
             , 0, 0
             , static_cast<int>(m_pAtlas->Width() * m_fmagni), static_cast<int>(m_pAtlas->Height() * m_fmagni)
             , m_pAtlas->GetDC()
             , 0
             , 0
             , static_cast<int>(m_pAtlas->Width()), static_cast<int>(m_pAtlas->Height())
             , SRCCOPY);

    TransparentBlt(_dc
                 , static_cast<int>(vPos.x + vDifPos.x * m_fmagni)
                 , static_cast<int>(vPos.y + vDifPos.y * m_fmagni)
                 , static_cast<int>(m_pAtlas->Width() * m_fmagni)
                 , static_cast<int>(m_pAtlas->Height() * m_fmagni)
                 , m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(m_pAtlas->Width() * m_fmagni), static_cast<int>(m_pAtlas->Height() * m_fmagni)
                 , RGB(255, 0, 255));

    CObj::render(_dc);
}
