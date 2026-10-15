#include "pch.h"
#include "Game\04.Object\BackGround\CBackGround_Back.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CBackGround_Back::CBackGround_Back()
    : m_pAtlas(nullptr)
    , m_pBackBuffer(nullptr)
    , m_vecResolution{}
    , m_fMagni(0.f)
    , m_dParallax(0.0)
    , m_iTop(0)
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"stage", L"background");
    m_fMagni                  = tData.GetFloat(L"far_magni");
    m_dParallax               = tData.GetDouble(L"far_parallax");
    m_iTop                    = tData.GetInt(L"far_top");

    m_vecResolution = CEngine::GetInst()->GetResolution();
    m_pBackBuffer   = CResMgr::GetInst()->CreateTexture(L"BackGround_BackBackBuffer", static_cast<UINT>(m_vecResolution.x), static_cast<UINT>(m_vecResolution.y));
    SetPos(Vec2(0.f, 0.f));
    m_pAtlas = CResMgr::GetInst()->LoadTexture(L"BackGround_Back", L"texture\\BackGround_Back.bmp");
}

CBackGround_Back::~CBackGround_Back()
{
}

void CBackGround_Back::tick()
{
    CObj::tick();
}

void CBackGround_Back::render(const HDC _dc)
{
    // 화면 왼쪽 위가 보는 배경 텍스처 좌표. 먼 배경이라 그보다 천천히(1/m_dParallax) 움직인다
    const Vec2 vPos = CCamera::GetInst()->GetRealPos(GetPos()) / BG_MAGNI;

    StretchBlt(m_pBackBuffer->GetDC()
             , 0, 0
             , static_cast<int>(m_vecResolution.x), static_cast<int>(m_vecResolution.y)
             , m_pAtlas->GetDC()
             , static_cast<int>(vPos.x / m_dParallax)
             , static_cast<int>(vPos.y / m_dParallax) + m_iTop
             , static_cast<int>(m_vecResolution.x / m_fMagni), static_cast<int>(m_vecResolution.y / m_fMagni)
             , SRCCOPY);

    TransparentBlt(_dc
                 , 0
                 , 0
                 , static_cast<int>(m_vecResolution.x)
                 , static_cast<int>(m_vecResolution.y)
                 , m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(m_vecResolution.x), static_cast<int>(m_vecResolution.y)
                 , RGB(255, 0, 255));

    CObj::render(_dc);
}
