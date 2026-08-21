#include "pch.h"
#include "Game\04.Object\UI\CUI.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

CUI::CUI()
    : m_pParentUI(nullptr)
    , m_pIdleTex(nullptr)
    , m_bMouseOn(false)
    , m_bLbtnDown(false)
    , m_bCmrAfcted(false)
{
}

CUI::CUI(const CUI& _other)
    : CObj(_other)
    , m_pParentUI(nullptr)
    , m_pIdleTex(_other.m_pIdleTex)
    , m_bMouseOn(false)
    , m_bLbtnDown(false)
    , m_bCmrAfcted(_other.m_bCmrAfcted)
{
    for (const auto ui : _other.m_vecChildUI)
        AddChildUI(ui->Clone());
}

CUI::~CUI()
{
    for (const auto ui : m_vecChildUI)
    {
        DEL(ui)
    }
}

void CUI::tick()
{
    m_vFinalPos = GetPos();

    if (m_pParentUI)
        m_vFinalPos += m_pParentUI->GetFinalPos();

    MouseOnCheck();

    for (const auto ui : m_vecChildUI)
        ui->tick();
}

void CUI::render(const HDC _dc)
{
    const HPEN   hPen       = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);
    const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

    const HPEN   hOriginPen   = static_cast<HPEN>(SelectObject(_dc, hPen));
    const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));

    Vec2 vPos = GetPos();
    if (m_bCmrAfcted)
        vPos = CCamera::GetInst()->GetRenderPos(vPos);

    const Vec2 vScale = GetScale();

    Rectangle(_dc
            , static_cast<int>(vPos.x)
            , static_cast<int>(vPos.y)
            , static_cast<int>(vPos.x + vScale.x)
            , static_cast<int>(vPos.y + vScale.y));

    SelectObject(_dc, hOriginPen);
    SelectObject(_dc, hOriginBrush);

    render_ChildUI(_dc);
}

void CUI::render_ChildUI(const HDC _dc) const
{
    for (const auto ui : m_vecChildUI)
        ui->render(_dc);
}

void CUI::SetIdleTex(CTexture* _pTex)
{
    m_pIdleTex = _pTex;

    if (nullptr != m_pIdleTex)
    {
        const Vec2 vScale = Vec2(static_cast<float>(m_pIdleTex->Width()), static_cast<float>(m_pIdleTex->Height()));
        SetScale(vScale);
    }
}

void CUI::MouseOnCheck()
{
    const Vec2 vPos      = GetFinalPos();
    const Vec2 vScale    = GetScale();
    Vec2       vMousePos = MOUSE_POS;

    if (m_bCmrAfcted)
        vMousePos = CCamera::GetInst()->GetRealPos(vMousePos);

    if (vPos.x <= vMousePos.x && vMousePos.x <= vPos.x + vScale.x
        && vPos.y <= vMousePos.y && vMousePos.y <= vPos.y + vScale.y)
    {
        m_bMouseOn = true;
    }
    else
    {
        m_bMouseOn = false;
    }
}
