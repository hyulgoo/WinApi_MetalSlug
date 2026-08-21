#include "pch.h"
#include "Game\04.Object\BackGround\CStartBG.h"

#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\01.Engine\CEngine.h"

CStartBG::CStartBG()
    : m_pAtlas(nullptr)
    , m_fmagni(4.f)
{
    m_vResolution = CEngine::GetInst()->GetResolution();
    m_pAtlas      = CResMgr::GetInst()->LoadTexture(L"Select", L"texture\\Select.bmp");

    SetPos(Vec2(0.f, 0.f));
}

CStartBG::~CStartBG()
{
}

void CStartBG::tick()
{
    CObj::tick();
}

void CStartBG::render(const HDC _dc)
{
    StretchBlt(_dc
             , 0, 0
             , static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
             , m_pAtlas->GetDC()
             , 0
             , 0
             , static_cast<int>(m_pAtlas->Width()), static_cast<int>(m_pAtlas->Height())
             , SRCCOPY);

    CObj::render(_dc);
}
