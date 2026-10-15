#include "pch.h"
#include "Game\04.Object\BackGround\CBackGround_Front.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\01.Engine\CEngine.h"

CBackGround_Front::CBackGround_Front(const wstring& _strKey, const wstring& _strPath, const Vec2& _vPos)
    : m_pAtlas(nullptr)
    , m_pScaled(nullptr)
    , m_vecResolution{}
{
    m_vecResolution = CEngine::GetInst()->GetResolution();
    m_pAtlas        = CResMgr::GetInst()->LoadTexture(_strKey, _strPath);

    // 변하지 않는 그림이라 한 번만 확대해 둔다
    const int iWidth  = static_cast<int>(m_pAtlas->Width() * BG_MAGNI);
    const int iHeight = static_cast<int>(m_pAtlas->Height() * BG_MAGNI);
    m_pScaled         = CResMgr::GetInst()->CreateTexture(_strKey + L"_Scaled", static_cast<UINT>(iWidth), static_cast<UINT>(iHeight));
    StretchBlt(m_pScaled->GetDC()
             , 0, 0, iWidth, iHeight
             , m_pAtlas->GetDC()
             , 0, 0, static_cast<int>(m_pAtlas->Width()), static_cast<int>(m_pAtlas->Height())
             , SRCCOPY);

    SetPos(_vPos);
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
    // 앞 배경 조각은 월드 좌표에 놓여 있다. 화면 밖이면 그리지 않는다
    const Vec2 vPos    = CCamera::GetInst()->GetRenderPos(GetPos());
    const int  iWidth  = static_cast<int>(m_pScaled->Width());
    const int  iHeight = static_cast<int>(m_pScaled->Height());
    if (vPos.x >= m_vecResolution.x || vPos.y >= m_vecResolution.y || vPos.x + iWidth <= 0.f || vPos.y + iHeight <= 0.f)
        return;

    TransparentBlt(_dc
                 , static_cast<int>(vPos.x)
                 , static_cast<int>(vPos.y)
                 , iWidth, iHeight
                 , m_pScaled->GetDC()
                 , 0, 0
                 , iWidth, iHeight
                 , RGB(255, 0, 255));

    CObj::render(_dc);
}
