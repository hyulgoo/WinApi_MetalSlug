#include "pch.h"
#include "CBackGround.h"

#include "CCamera.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CBackGround::CBackGround()
    : m_pAtlas(nullptr)
    , m_pBackBuffer(nullptr)
    , m_fmagni(4.f)
{
    m_vResolution = CEngine::GetInst()->GetResolution();
    m_pAtlas      = CResMgr::GetInst()->LoadTexture(L"BackGroundasd", L"texture\\BackGround.bmp");
    m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_BackBuffer", static_cast<UINT>(m_vResolution.x + 150), static_cast<UINT>(m_vResolution.y));
    SetPos(Vec2(0.f, 0.f));
}

CBackGround::~CBackGround()
{
}

void CBackGround::tick()
{
    CObj::tick();
}

void CBackGround::render(const HDC _dc)
{
    Vec2 vPos = GetPos();
    vPos      = CCamera::GetInst()->GetRealPos(vPos);

    StretchBlt(m_pBackBuffer->GetDC()
             , 0, 0
             , static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
             , m_pAtlas->GetDC()
             , static_cast<int>(vPos.x)
             , static_cast<int>(vPos.y)
             , static_cast<int>(m_vResolution.x / m_fmagni), static_cast<int>(m_vResolution.y / m_fmagni)
             , SRCCOPY);

    TransparentBlt(_dc
                 , 0
                 , 0
                 , static_cast<int>(m_vResolution.x)
                 , static_cast<int>(m_vResolution.y)
                 , m_pBackBuffer->GetDC()
                 , 0, 0
                 , static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
                 , RGB(255, 0, 255));

    CObj::render(_dc);
}
