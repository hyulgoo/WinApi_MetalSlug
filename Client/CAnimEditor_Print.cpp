#include "pch.h"
#include "CAnimEditor.h"
#include "CAnimation.h"
#include "CAnimator.h"

#include "CEngine.h"
#include "CEditorButton.h"
#include "CEditorPanel.h"

#include "CKeyMgr.h"
#include "CCamera.h"

#include "CEditAnim.h"
#include "CMouseArea.h"
#include "CEditPlayer.h"

void CAnimEditor::PrintCreateAnimation()
{
    if (IsTap(KEY::F1))
    {
        if (m_tPrintMode == PRINTMODE::UPPER)
        {
            const vector<CObj*> vector = GetLayer(LAYER::EDITUI);
            if (!vector.empty())
            {
                if (m_pEditPlayer->GetAnimator()->GetAnimation() != nullptr)
                {
                    if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"확인", MB_YESNO) == IDNO)
                        return;
                }
                DeleteObject(LAYER::EDITUI);

                if (!m_vecWorkUpperFrm.empty())
                {
                    assert(m_iCreatFrmCount == m_vecWorkUpperFrm.size());
                    m_pEditPlayer->GetAnimator()->CreateEditUpperAnimation(m_vecWorkUpperFrm, m_pAtlas);
                    m_pEditPlayer->GetAnimator()->Play(L"WorkAnim", true);
                    m_pEditPlayer->GetAnimator()->GetAnimation()->SetUpper();
                    m_bAnimator1      = true;
                    m_vecPrevUpperFrm = m_vecWorkUpperFrm;
                    m_iCreatFrmCount  = 0;
                    m_vecWorkUpperFrm.clear();
                }
            }
        }

        else if (m_tPrintMode == PRINTMODE::LOWER)
        {
            const vector<CObj*> vector = GetLayer(LAYER::EDITUI);
            if (!vector.empty())
            {
                if (m_pEditPlayer->GetAnimator2()->GetAnimation() != nullptr)
                {
                    if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"확인", MB_YESNO) == IDNO)
                        return;
                }
                DeleteObject(LAYER::EDITUI);
                if (!m_vecWorkLowerFrm.empty())
                {
                    assert(m_iCreatFrmCount == m_vecWorkLowerFrm.size());
                    m_pEditPlayer->GetAnimator2()->CreateEditLowerAnimation(m_vecWorkLowerFrm, m_pAtlas);
                    m_pEditPlayer->GetAnimator2()->Play(L"WorkAnim1", true);
                    m_pEditPlayer->GetAnimator2()->GetAnimation()->SetLower();
                    m_bAnimator2      = true;
                    m_vecPrevUpperFrm = m_vecWorkLowerFrm;
                    m_iCreatFrmCount  = 0;
                    m_vecWorkLowerFrm.clear();
                }
            }
        }
        else if (m_tPrintMode == PRINTMODE::NORMAL)
        {
            const vector<CObj*> vector = GetLayer(LAYER::EDITUI);
            if (!vector.empty())
            {
                if (m_pEditPlayer->GetAnimator()->GetAnimation() != nullptr)
                {
                    if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"확인", MB_YESNO) == IDNO)
                        return;
                }
                DeleteObject(LAYER::EDITUI);
                if (!m_vecWorkNormalFrm.empty())
                {
                    assert(m_iCreatFrmCount == m_vecWorkNormalFrm.size());
                    m_pEditPlayer->GetAnimator()->CreateEditUpperAnimation(m_vecWorkNormalFrm, m_pAtlas);
                    m_pEditPlayer->GetAnimator()->Play(L"WorkAnim", true);
                    m_bAnimator1       = true;
                    m_vecPrevNormalFrm = m_vecWorkNormalFrm;
                    m_iCreatFrmCount   = 0;
                    m_vecWorkNormalFrm.clear();
                }
            }
        }
    }
}

void CAnimEditor::PrintDragArea()
{
    if (IsTap(KEY::LBTN))
    {
        m_bIsStarted           = true;
        const Vec2 MousetapPos = MOUSE_POS;
        const Vec2 RealtapPos  = CCamera::GetInst()->GetRealPos(MousetapPos) / m_fmagni;
        m_ptRealStart.x        = static_cast<int>(RealtapPos.x);
        m_ptRealStart.y        = static_cast<int>(RealtapPos.y);
        m_ptStart.x            = static_cast<int>(MousetapPos.x);
        m_ptStart.y            = static_cast<int>(MousetapPos.y);
    }

    if (m_bIsStarted == true)
    {
        if (IsPressed(KEY::LBTN))
        {
            const Vec2 MousePressPos = MOUSE_POS;
            const Vec2 RealPressPos  = CCamera::GetInst()->GetRealPos(MousePressPos) / m_fmagni;
            m_ptRealEnd.x            = static_cast<int>(RealPressPos.x);
            m_ptRealEnd.y            = static_cast<int>(RealPressPos.y);

            m_bIsPressed      = true;
            m_ptEnd.x         = static_cast<int>(MousePressPos.x);
            m_ptEnd.y         = static_cast<int>(MousePressPos.y);
            m_vecDragRectSize = Vec2(static_cast<float>(m_ptRealEnd.x - m_ptRealStart.x), static_cast<float>(m_ptRealEnd.y - m_ptRealStart.y));
        }
        if (IsRelease(KEY::LBTN))
        {
            const Vec2 MouseReleasePos = MOUSE_POS;
            const Vec2 RealReleasePos  = CCamera::GetInst()->GetRealPos(MouseReleasePos) / m_fmagni;
            m_ptRealEnd.x              = static_cast<int>(RealReleasePos.x);
            m_ptRealEnd.y              = static_cast<int>(RealReleasePos.y);
            m_ptEnd.x                  = static_cast<int>(MouseReleasePos.x);
            m_ptEnd.y                  = static_cast<int>(MouseReleasePos.y);
            m_vecDragRectSize          = Vec2(static_cast<float>(m_ptRealEnd.x - m_ptRealStart.x), static_cast<float>(m_ptRealEnd.y - m_ptRealStart.y));

            CMouseArea* DragArea = new CMouseArea;
            DragArea->SetStartEnd(m_ptRealStart, m_ptRealEnd);
            DragArea->SetMagni(m_fmagni);
            const Vec2 vPos = Vec2(m_ptRealStart.x - m_ptRealEnd.x / 2.f, m_ptRealStart.y - m_ptRealEnd.y / 2.f);
            Instantiate(DragArea
                      , vPos
                      , LAYER::EDITUI);

            CreateAnimation();

            m_bIsStarted = false;
            ++m_iCreatFrmCount;
        }
    }
}

void CAnimEditor::CreateUI()
{
    m_pEditPanel = new CEditorPanel;
    m_pEditPanel->SetPos(Vec2(m_vResolution.x - m_vResolution.x / 4, 0));
    m_pEditPanel->SetScale(Vec2(m_vResolution.x / 4, m_vResolution.y));

    const Vec2 PanelScale = m_pEditPanel->GetScale();

    CEditorButton* pSaveUpperAnim = new CEditorButton;
    pSaveUpperAnim->SetPos(Vec2(PanelScale.x - 195, 10.f));
    pSaveUpperAnim->SetScale(Vec2(90.f, 30.f));
    pSaveUpperAnim->SetButtonTex(L"SaveUpper");
    pSaveUpperAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::SaveUpperAnimation));

    CEditorButton* pLoadUpperAnim = pSaveUpperAnim->Clone();
    pLoadUpperAnim->SetPos(Vec2(PanelScale.x - 95, 10.f));
    pLoadUpperAnim->SetButtonTex(L"LoadUpper");
    pLoadUpperAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::LoadUpperAnimation));

    CEditorButton* pSaveLowerAnim = pLoadUpperAnim->Clone();
    pSaveLowerAnim->SetPos(Vec2(PanelScale.x - 195, 50.f));
    pSaveLowerAnim->SetButtonTex(L"SaveLower");
    pSaveLowerAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::SaveLowerAnimation));

    CEditorButton* pLoadLowerAnim = pSaveLowerAnim->Clone();
    pLoadLowerAnim->SetPos(Vec2(PanelScale.x - 95, 50.f));
    pLoadLowerAnim->SetButtonTex(L"LoadLower");
    pLoadLowerAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::LoadLowerAnimation));

    CEditorButton* pSaveNormalAnim = pLoadLowerAnim->Clone();
    pSaveNormalAnim->SetPos(Vec2(PanelScale.x - 195, 90.f));
    pSaveNormalAnim->SetButtonTex(L"SaveNormal");
    pSaveNormalAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::SaveNormalAnimation));

    CEditorButton* pLoadNormalAnim = pSaveNormalAnim->Clone();
    pLoadNormalAnim->SetPos(Vec2(PanelScale.x - 95, 90.f));
    pLoadNormalAnim->SetButtonTex(L"LoadNormal");
    pLoadNormalAnim->SetDelegate(this, reinterpret_cast<DELEGATE>(&CAnimEditor::LoadNormalAnimation));

    m_pEditPanel->AddChildUI(pSaveUpperAnim);
    m_pEditPanel->AddChildUI(pLoadUpperAnim);
    m_pEditPanel->AddChildUI(pSaveLowerAnim);
    m_pEditPanel->AddChildUI(pLoadLowerAnim);
    m_pEditPanel->AddChildUI(pSaveNormalAnim);
    m_pEditPanel->AddChildUI(pLoadNormalAnim);
    AddObject(m_pEditPanel, LAYER::UI);
}

void CAnimEditor::PrintMouseInfo(const HDC _dc) const
{
    // 드래그가 시작됐다면 필요한 좌표들을 출력해준다.
    if (m_bIsStarted == true)
    {
        const HBRUSH hNullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        const HPEN   hPen       = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);

        // DC 에 원래 있던 펜과 브러시를 저장해두고 그리는데 쓸 것들로 교체한다
        const HPEN   hOriginPen   = static_cast<HPEN>(SelectObject(_dc, hPen));
        const HBRUSH hOriginBrush = static_cast<HBRUSH>(SelectObject(_dc, hNullBrush));
        Rectangle(CEngine::GetInst()->GetMemTexDC()
                , static_cast<int>(m_ptStart.x)
                , static_cast<int>(m_ptStart.y)
                , static_cast<int>(m_ptEnd.x)
                , static_cast<int>(m_ptEnd.y));

        SelectObject(_dc, hOriginPen);
        SelectObject(_dc, hOriginBrush);

        SetTextAlign(_dc, TA_LEFT);

        TCHAR StartmousePos[128] = {};
        wsprintf(StartmousePos, TEXT("Letf : %d / Top : %d"), static_cast<int>(m_ptRealStart.x), static_cast<int>(m_ptRealStart.y));
        TCHAR EndmousePos[128] = {};
        wsprintf(EndmousePos, TEXT("Right : %d / Bottom : %d"), static_cast<int>(m_ptRealEnd.x), static_cast<int>(m_ptRealEnd.y));
        TCHAR RectSize[128] = {};
        wsprintf(RectSize, TEXT("Width : %d / Height : %d"), static_cast<UINT>(m_vecDragRectSize.x), static_cast<UINT>(m_vecDragRectSize.y));

        TextOut(_dc, 40, 60, StartmousePos, lstrlenW(StartmousePos));
        TextOut(_dc, 40, 80, EndmousePos, lstrlenW(EndmousePos));
        TextOut(_dc, 40, 100, RectSize, lstrlenW(RectSize));
    }
}

void CAnimEditor::PrintInfo(const HDC _dc) const
{
    SetTextAlign(_dc, TA_LEFT);
    constexpr LPCTSTR Explan  = L"Z : FirstFrm  //  X, C : Prev, NextFrm ";
    constexpr LPCTSTR Explan1 = L"Q, E : Duration ";
    constexpr LPCTSTR Explan2 = L"WASD : LeftTop  //  WASD + LSHIFT : Offset ";
    constexpr LPCTSTR Explan3 = L"WASD + LCTRL : Size";
    constexpr LPCTSTR Explan4 = L"SPACE : PLAY / STOP";

    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), static_cast<int>(m_vResolution.y) - 100, Explan, lstrlenW(Explan));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), static_cast<int>(m_vResolution.y) - 80, Explan1, lstrlenW(Explan1));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), static_cast<int>(m_vResolution.y) - 60, Explan2, lstrlenW(Explan2));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), static_cast<int>(m_vResolution.y) - 40, Explan3, lstrlenW(Explan3));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), static_cast<int>(m_vResolution.y) - 20, Explan4, lstrlenW(Explan4));
}

void CAnimEditor::PrintEditAnimInfo(const HDC _dc, CAnimator* pAnimator) const
{
    // 현재 편집 중인 애니메이션 정보 출력
    const LPCTSTR          AnimName = pAnimator->GetAnimation()->GetName().c_str();
    LPCTSTR                Explan   = L"z : 처음Frm  x, c : 이전, 다음 Frm ";
    const vector<tAnimFrm> FrmInfo  = pAnimator->GetAnimation()->GetFrmInfo();
    const tAnimFrm         CurFrm   = pAnimator->GetAnimation()->GetCurFrm();

    TCHAR Frm[32] = {};
    wsprintf(Frm, TEXT("FrmCount : %d"), static_cast<int>(FrmInfo.size()));

    TCHAR CurFrmNum[32] = {};
    wsprintf(CurFrmNum, TEXT("CurFrm : %d"), pAnimator->GetAnimation()->GetCurFrmNumber() + 1);

    TCHAR LeftTop[32] = {};
    wsprintf(LeftTop, TEXT("LeftTop : x.%d,  y.%d"), static_cast<int>(CurFrm.vLeftTop.x), static_cast<int>(CurFrm.vLeftTop.y));

    TCHAR Size[32] = {};
    wsprintf(Size, TEXT("size : x.%d, y.%d "), static_cast<int>(CurFrm.vSize.x), static_cast<int>(CurFrm.vSize.y));

    TCHAR Offset[32] = {};
    wsprintf(Offset, TEXT("Offset : x.%d, y.%d"), static_cast<int>(CurFrm.vOffset.x), static_cast<int>(CurFrm.vOffset.y));

    TCHAR Duration[32] = {};
    wsprintf(Duration, TEXT("Duration : %d"), static_cast<int>(CurFrm.fDuration * 100));

    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 10, AnimName, lstrlenW(AnimName));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 30, Frm, lstrlenW(Frm));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 50, CurFrmNum, lstrlenW(CurFrmNum));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 70, LeftTop, lstrlenW(LeftTop));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 90, Size, lstrlenW(Size));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 110, Offset, lstrlenW(Offset));
    TextOut(_dc, static_cast<int>(m_vResolution.x - (m_vResolution.x / 4) + 10), 130, Duration, lstrlenW(Duration));
}

void CAnimEditor::PrintMousePos(const HDC _dc) const
{
    const Vec2 MousePressPos = MOUSE_POS;
    const Vec2 RealPressPos  = CCamera::GetInst()->GetRealPos(MousePressPos) / m_fmagni;

    TCHAR MousePos[128] = {};
    wsprintf(MousePos, TEXT("X : %d / Y : %d"), static_cast<UINT>(RealPressPos.x), static_cast<UINT>(RealPressPos.y));

    SetTextAlign(_dc, TA_LEFT);
    TextOut(_dc, 20, static_cast<int>(m_vResolution.y) - 20, MousePos, lstrlenW(MousePos));
}

void CAnimEditor::PlayLeft() const
{
    if (m_bAnimator1)
        m_pEditPlayer->GetAnimator()->GetAnimation()->SetPlayLeft();
    if (m_bAnimator2)
        m_pEditPlayer->GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CAnimEditor::PlayRight() const
{
    if (m_bAnimator1)
        m_pEditPlayer->GetAnimator()->GetAnimation()->SetPlayRight();
    if (m_bAnimator2)
        m_pEditPlayer->GetAnimator2()->GetAnimation()->SetPlayRight();
}
