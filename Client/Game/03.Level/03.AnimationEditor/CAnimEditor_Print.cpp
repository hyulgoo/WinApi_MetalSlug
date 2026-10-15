#include "pch.h"
#include "Game\03.Level\03.AnimationEditor\CAnimEditor.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\04.Object\UI\ButtonUI\CEditorButton.h"
#include "Game\04.Object\UI\PanelUI\CEditorPanel.h"
#include "Game\04.Object\UI\EditAnim\EditPlayer\CEditPlayer.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\06.Resource\Texture\CTexture.h"

#include <cmath>

namespace
{
    const wchar_t* const DOC_ANIM = L"EditorDoc";   // 미리보기 애니메이터 안에서 쓰는 이름
    const wchar_t* const REF_ANIM = L"EditorRef";

    SETPRINT ToPrint(const CAnimEditor::PREVIEW _ePreview)
    {
        if (CAnimEditor::PREVIEW::UPPER == _ePreview)
            return SETPRINT::UPPER;
        if (CAnimEditor::PREVIEW::LOWER == _ePreview)
            return SETPRINT::LOWER;
        return SETPRINT::NORMAL;
    }

    const wchar_t* PreviewName(const CAnimEditor::PREVIEW _ePreview)
    {
        if (CAnimEditor::PREVIEW::UPPER == _ePreview)
            return L"UPPER";
        if (CAnimEditor::PREVIEW::LOWER == _ePreview)
            return L"LOWER";
        return L"NORMAL";
    }

    void DrawRect(const HDC _dc, const PEN_TYPE _ePen, const Vec2& _vLeftTop, const Vec2& _vRightBottom)
    {
        const HPEN   hOldPen   = static_cast<HPEN>(SelectObject(_dc, CEngine::GetInst()->GetPen(_ePen)));
        const HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(_dc, GetStockObject(NULL_BRUSH)));
        Rectangle(_dc
                , static_cast<int>(_vLeftTop.x), static_cast<int>(_vLeftTop.y)
                , static_cast<int>(_vRightBottom.x), static_cast<int>(_vRightBottom.y));
        SelectObject(_dc, hOldPen);
        SelectObject(_dc, hOldBrush);
    }

    // 한 줄 쓰고 다음 줄로
    void TextLine(const HDC _dc, const int _iX, int& _iY, const int _iLineHeight, const wchar_t* _szText)
    {
        TextOut(_dc, _iX, _iY, _szText, lstrlen(_szText));
        _iY += _iLineHeight;
    }
}

void CAnimEditor::CreateUI()
{
    m_pPanel = new CEditorPanel;
    m_pPanel->SetPos(Vec2(m_fPanelLeft, 0.f));
    m_pPanel->SetScale(Vec2(m_vResolution.x - m_fPanelLeft, m_vResolution.y));

    // 버튼 3열 4줄. 누르면 이 에디터의 명령을 부른다
    struct tButton
    {
        const wchar_t*        szText;
        std::function<void()> fnClick;
    };
    const tButton arrButton[] =
    {
        { L"New",      [this] { NewDoc(); } },
        { L"Load",     [this] { Load(); } },
        { L"Save",     [this] { Save(); } },
        { L"< Sheet",  [this] { ChangeSheet(-1); } },
        { L"Sheet >",  [this] { ChangeSheet(1); } },
        { L"Open BMP", [this] { OpenSheetFile(); } },
        { L"Grid Add", [this] { OpenGridDialog(); } },
        { L"Ref Load", [this] { LoadReference(); } },
        { L"Ref Off",  [this] { ClearReference(); } },
        { L"Delete",   [this] { DeleteFrame(); } },
        { L"Undo",     [this] { Undo(); } },
        { L"Title",    [this] { GoTitle(); } },
    };
    for (int i = 0; i < static_cast<int>(std::size(arrButton)); ++i)
    {
        CEditorButton* pButton = new CEditorButton;
        const int iPerRow = max(1, m_tData.iButtonsPerRow);
        pButton->SetPos(Vec2(m_tData.fMargin + static_cast<float>(i % iPerRow) * (m_tData.vButtonSize.x + m_tData.fButtonGap)
                           , m_tData.fMargin + static_cast<float>(i / iPerRow) * (m_tData.vButtonSize.y + m_tData.fButtonGap)));
        pButton->SetScale(m_tData.vButtonSize);
        pButton->SetText(arrButton[i].szText);
        pButton->SetOnClick(arrButton[i].fnClick);
        m_pPanel->AddChildUI(pButton);
    }
    AddObject(m_pPanel, LAYER::UI);
}

void CAnimEditor::SyncPreview(const bool _bReference)
{
    if (nullptr == m_pPreview)
        return;

    // 편집 중인 애니메이션: 프레임을 통째로 넘긴다 (재생 위치는 이어 간다)
    CAnimator*  pDocAnimator = m_pPreview->GetAnimator();
    CAnimation* pDoc         = pDocAnimator->FindAnimation(DOC_ANIM);
    if (nullptr == pDoc)
    {
        pDoc = pDocAnimator->CreateFrameAnimation(DOC_ANIM, m_pSheet, m_tDoc.vecFrm);
        pDocAnimator->Play(DOC_ANIM, true);
    }
    else
    {
        pDoc->SetAtlas(m_pSheet);
        pDoc->SetAllFrmInfo(m_tDoc.vecFrm);
        if (pDoc->IsFinish())
            pDoc->Reset();
    }
    pDoc->SetPrintMode(ToPrint(m_ePreview));
    if (m_bPreviewLeft)
        pDoc->SetPlayLeft();
    else
        pDoc->SetPlayRight();

    // 참고 애니메이션 (읽기만 한다)
    CAnimator* pRefAnimator = m_pPreview->GetAnimator2();
    if (_bReference)
    {
        pRefAnimator->DeleteAnimation(REF_ANIM);
        if (!m_tRef.vecFrm.empty())
        {
            CTexture* pAtlas = CResMgr::GetInst()->LoadTexture(m_tRef.strAtlasKey, m_tRef.strAtlasPath);
            pRefAnimator->CreateFrameAnimation(REF_ANIM, pAtlas, m_tRef.vecFrm);
            pRefAnimator->Play(REF_ANIM, true);
        }
    }
    if (CAnimation* pRef = pRefAnimator->GetAnimation())
    {
        pRef->SetPrintMode(ToPrint(m_eRefPreview));
        if (m_bPreviewLeft)
            pRef->SetPlayLeft();
        else
            pRef->SetPlayRight();
    }

    m_pPreview->SetPause(m_bPause);
    m_pPreview->SetDocBelow(PREVIEW::LOWER == m_ePreview);
}

void CAnimEditor::render(const HDC _dc)
{
    // 시트와 프레임 사각형 → 패널·버튼·미리보기(레벨 오브젝트) → 글자
    RenderSheet(_dc);
    RenderFrames(_dc);
    CLevel::render(_dc);
    RenderInfo(_dc);
}

void CAnimEditor::RenderSheet(const HDC _dc) const
{
    if (nullptr == m_pSheet)
        return;

    const Vec2  vOrigin = SheetOrigin();
    const float fMagni  = m_fSheetMagni;
    const int   iWidth  = static_cast<int>(m_pSheet->Width());
    const int   iHeight = static_cast<int>(m_pSheet->Height());

    // 편집 영역에 보이는 시트 픽셀만 그린다
    // (예전에는 3배로 키운 시트 전체를 매 프레임 그렸다. Eri.bmp는 2748x23613이고 대부분 화면 밖이다)
    const int iLeft   = max(0, static_cast<int>(floorf(-vOrigin.x / fMagni)));
    const int iTop    = max(0, static_cast<int>(floorf(-vOrigin.y / fMagni)));
    const int iRight  = min(iWidth, static_cast<int>(ceilf((m_fPanelLeft - vOrigin.x) / fMagni)));
    const int iBottom = min(iHeight, static_cast<int>(ceilf((m_vResolution.y - vOrigin.y) / fMagni)));
    if (iRight > iLeft && iBottom > iTop)
    {
        StretchBlt(_dc
                 , static_cast<int>(vOrigin.x + static_cast<float>(iLeft) * fMagni)
                 , static_cast<int>(vOrigin.y + static_cast<float>(iTop) * fMagni)
                 , static_cast<int>(static_cast<float>(iRight - iLeft) * fMagni)
                 , static_cast<int>(static_cast<float>(iBottom - iTop) * fMagni)
                 , m_pSheet->GetDC()
                 , iLeft, iTop, iRight - iLeft, iBottom - iTop
                 , SRCCOPY);
    }

    // 시트 테두리
    DrawRect(_dc, PEN_TYPE::BLUE, vOrigin - Vec2(1.f, 1.f), vOrigin + Vec2(static_cast<float>(iWidth), static_cast<float>(iHeight)) * fMagni + Vec2(1.f, 1.f));
}

void CAnimEditor::RenderFrames(const HDC _dc) const
{
    if (nullptr == m_pSheet)
        return;

    const Vec2 vOrigin  = SheetOrigin();
    auto       ToScreen = [&](const Vec2& _vPixel) { return vOrigin + _vPixel * m_fSheetMagni; };

    const int      iOldMode  = SetBkMode(_dc, TRANSPARENT);
    const COLORREF tOldColor = SetTextColor(_dc, m_tData.tSheetTextColor);
    SetTextAlign(_dc, TA_LEFT);

    // 모든 프레임 (흰색, 번호는 1부터) → 고르고 있는 프레임 (빨강, 위에 다시)
    // 예전에는 드래그할 때마다 사각형 오브젝트(CMouseArea)를 만들어 두었고 프레임을 지워도 남았다
    wchar_t szText[64] = {};
    for (int i = 0; i < static_cast<int>(m_tDoc.vecFrm.size()); ++i)
    {
        const tAnimFrm& frm          = m_tDoc.vecFrm[i];
        const Vec2      vLeftTop     = ToScreen(frm.vLeftTop);
        const Vec2      vRightBottom = ToScreen(frm.vLeftTop + frm.vSize);
        if (vRightBottom.x < 0.f || vLeftTop.x > m_fPanelLeft || vRightBottom.y < 0.f || vLeftTop.y > m_vResolution.y)
            continue;

        DrawRect(_dc, i == m_iCurFrm ? PEN_TYPE::RED : PEN_TYPE::WHITE, vLeftTop, vRightBottom);
        swprintf_s(szText, L"%d", i + 1);
        TextOut(_dc, static_cast<int>(vLeftTop.x) + static_cast<int>(m_tData.vFrameNumberOffset.x), static_cast<int>(vLeftTop.y) + static_cast<int>(m_tData.vFrameNumberOffset.y), szText, lstrlen(szText));
    }
    if (!m_tDoc.vecFrm.empty())
    {
        const tAnimFrm& frm = m_tDoc.vecFrm[m_iCurFrm];
        DrawRect(_dc, PEN_TYPE::RED, ToScreen(frm.vLeftTop), ToScreen(frm.vLeftTop + frm.vSize));
    }

    // 끌고 있는 사각형과 크기
    if (m_bDrag)
    {
        const Vec2 vLeftTop(min(m_vDragStart.x, m_vDragEnd.x), min(m_vDragStart.y, m_vDragEnd.y));
        const Vec2 vRightBottom(max(m_vDragStart.x, m_vDragEnd.x), max(m_vDragStart.y, m_vDragEnd.y));
        DrawRect(_dc, PEN_TYPE::GREEN, ToScreen(vLeftTop), ToScreen(vRightBottom));

        const Vec2 vTextPos = ToScreen(vRightBottom) + m_tData.vDragTextOffset;
        swprintf_s(szText, L"%d x %d", static_cast<int>(vRightBottom.x - vLeftTop.x), static_cast<int>(vRightBottom.y - vLeftTop.y));
        TextOut(_dc, static_cast<int>(vTextPos.x), static_cast<int>(vTextPos.y), szText, lstrlen(szText));
    }

    SetTextColor(_dc, tOldColor);
    SetBkMode(_dc, iOldMode);
}

void CAnimEditor::RenderInfo(const HDC _dc) const
{
    const int iOldMode = SetBkMode(_dc, TRANSPARENT);
    SetTextAlign(_dc, TA_LEFT);
    SetTextColor(_dc, m_tData.tInfoColor);

    const int iX            = static_cast<int>(m_fPanelLeft + m_tData.fMargin);
    int       iY            = m_tData.iInfoTop;
    wchar_t   szLine[512]   = {};

    // 시트
    if (nullptr != m_pSheet)
        swprintf_s(szLine, L"Sheet: %s (%ux%u) x%d", m_tDoc.strAtlasKey.c_str(), m_pSheet->Width(), m_pSheet->Height(), static_cast<int>(m_fSheetMagni));
    else
        swprintf_s(szLine, L"Sheet: (none)");
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    swprintf_s(szLine, L"  %s", m_tDoc.strAtlasPath.c_str());
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);

    // 애니메이션 (이름은 저장한 파일 이름. *는 저장하지 않은 변경)
    swprintf_s(szLine, L"Anim: %s%s", m_tDoc.strName.empty() ? L"(new)" : m_tDoc.strName.c_str(), IsDirty() ? L" *" : L"");
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    if (m_tDoc.vecFrm.empty())
    {
        TextLine(_dc, iX, iY, m_tData.iLineHeight, L"Frame: - / 0  (drag on the sheet)");
    }
    else
    {
        const tAnimFrm& frm = m_tDoc.vecFrm[m_iCurFrm];
        swprintf_s(szLine, L"Frame: %d / %d", m_iCurFrm + 1, static_cast<int>(m_tDoc.vecFrm.size()));
        TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
        swprintf_s(szLine, L"LeftTop: %g, %g", frm.vLeftTop.x, frm.vLeftTop.y);
        TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
        swprintf_s(szLine, L"Size: %g, %g", frm.vSize.x, frm.vSize.y);
        TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
        swprintf_s(szLine, L"Offset: %g, %g", frm.vOffset.x, frm.vOffset.y);
        TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
        swprintf_s(szLine, L"Duration: %.2f", frm.fDuration);
        TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    }

    // 미리보기
    swprintf_s(szLine, L"Preview: %s %s %s", PreviewName(m_ePreview), m_bPreviewLeft ? L"LEFT" : L"RIGHT", m_bPause ? L"PAUSED" : L"PLAYING");
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    swprintf_s(szLine, L"Offset all frames (F2): %s", m_bOffsetAll ? L"ON" : L"OFF");
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    swprintf_s(szLine, L"Ref: %s", m_tRef.vecFrm.empty() ? L"(none)" : m_tRef.strName.c_str());
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);
    swprintf_s(szLine, L"Undo: %d", static_cast<int>(m_vecUndo.size()));
    TextLine(_dc, iX, iY, m_tData.iLineHeight, szLine);

    // 조작법 (패널 아래쪽. 예전 안내에는 F1~F5·TAB이 없었다)
    static const wchar_t* const arrHelp[] =
    {
        L"L-drag: add frame  click: select",
        L"R-drag, arrows: scroll  1-4: zoom",
        L"Z: first  X/C: prev/next frame",
        L"J/L: reorder  DEL: delete frame",
        L"Q/E: duration  Ctrl+Z: undo",
        L"WASD: left-top  Ctrl+WASD: size",
        L"Shift+WASD: offset  F2: all frames",
        L"TAB: normal/upper/lower preview",
        L"R/F: left/right  SPACE: pause",
        L"F5: new  F4: title  F1: hide help",
    };
    const int iHelpCount = m_bHelp ? static_cast<int>(std::size(arrHelp)) : 1;
    iY                   = static_cast<int>(m_vResolution.y) - static_cast<int>(m_tData.fMargin) - iHelpCount * m_tData.iLineHeight;
    if (m_bHelp)
    {
        for (const wchar_t* szHelp : arrHelp)
            TextLine(_dc, iX, iY, m_tData.iLineHeight, szHelp);
    }
    else
    {
        TextLine(_dc, iX, iY, m_tData.iLineHeight, L"F1: show help");
    }

    // 마우스 아래 시트 픽셀 (편집 영역 왼쪽 아래)
    const Vec2 vMouse = MOUSE_POS;
    if (nullptr != m_pSheet && IsOnSheetArea(vMouse))
    {
        const Vec2 vPixel = ToSheetPixel(vMouse);
        swprintf_s(szLine, L"x %d, y %d", static_cast<int>(vPixel.x), static_cast<int>(vPixel.y));
        SetBkMode(_dc, OPAQUE);
        // y가 음수면 화면 아래에서 잰다
        const Vec2 vText = m_tData.vMouseTextPos;
        TextOut(_dc, static_cast<int>(vText.x), static_cast<int>(vText.y < 0.f ? m_vResolution.y + vText.y : vText.y), szLine, lstrlen(szLine));
    }

    SetBkMode(_dc, iOldMode);
}
