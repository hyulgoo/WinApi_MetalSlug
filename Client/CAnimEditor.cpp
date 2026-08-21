#include "pch.h"
#include "CAnimEditor.h"
#include "CAnimation.h"
#include "CAnimator.h"

#include "CEngine.h"
#include "CEditorPanel.h"
#include "CTexture.h"

#include "CKeyMgr.h"
#include "CCamera.h"
#include "CTimeMgr.h"

#include "CEditAnim.h"
#include "CEditPlayer.h"

#include "resource.h"
#include "CLevelMgr.h"

#define GETCURFRM(pAnimator) tAnimFrm CurFrm = pAnimator->GetAnimation()->GetCurFrm()
#define GETALLFRM(pAnimator) vector<tAnimFrm> AllFrm = pAnimator->GetAnimation()->GetFrmInfo()

CAnimEditor::CAnimEditor()
    : m_hMenu(nullptr)
    , m_pEditImage(nullptr)
    , m_vecPrevUpperFrm{}
    , m_vecWorkUpperFrm{}
    , m_vecPrevLowerFrm{}
    , m_vecWorkLowerFrm{}
    , m_bIsStarted(false)
    , m_bIsPressed(false)
    , m_ptRealStart()
    , m_ptRealEnd()
    , m_ptStart()
    , m_ptEnd()
    , m_vecDragRectSize()
    , m_vResolution()
    , m_fmagni()
    , m_iCreatFrmCount(0)
    , m_pEditPanel(nullptr)
    , m_fKeyPressDelay(0.f)
    , m_tPrintMode(PRINTMODE::UPPER)
    , m_bAnimator1(false)
    , m_bAnimator2(false)
    , m_bPressMode(false)
    , m_bOffsetPress(false)
    , m_pAtlas(nullptr)
    , m_pAnimator(nullptr)
{
    m_vResolution = CEngine::GetInst()->GetResolution();

    m_pEditPlayer = new CEditPlayer;
    m_pAtlas      = m_pEditPlayer->GetAtlas();
    AddObject(m_pEditPlayer, LAYER::EDITANIM);
    m_pAnimator = m_pEditPlayer->GetAnimator();
}

CAnimEditor::~CAnimEditor()
{
    if (m_hMenu != nullptr)
        DestroyMenu(m_hMenu);
}

void CAnimEditor::init()
{
    CCamera::GetInst()->SetCameraSpeed(1500);

    m_pEidtAnim = new CEditAnim;
    m_pEidtAnim->SetPos(Vec2(0.f, 0.f));
    AddObject(m_pEidtAnim, LAYER::BACKGROUND);
    m_fmagni = m_pEidtAnim->GetMagni();

    CreateUI();

    const Vec2 vResolution = CEngine::GetInst()->GetResolution();
    CCamera::GetInst()->SetLook(vResolution / 2.f);
}

void CAnimEditor::tick()
{
    CLevel::tick();

    PrintCreateAnimation();

    PrintDragArea();

    // 시작 화면으로 이동
    if (IsTap(KEY::F4))
        ChangeLevel(LEVEL_TYPE::START);

    // 프레스(누름) 조작 모드 전환
    if (IsTap(KEY::F2))
        m_bPressMode = !m_bPressMode;

    if (IsTap(KEY::F3))
        m_bOffsetPress = !m_bOffsetPress;

    // 상하체 애니메이터 전환
    if (IsTap(KEY::TAB))
    {
        //if (m_tPrintMode == PRINTMODE::NORMAL)
        //{
        //	m_tPrintMode = PRINTMODE::UPPER;
        //	m_pAnimator = m_pEditPlayer->GetAnimator();
        //}
        if (m_tPrintMode == PRINTMODE::UPPER)
        {
            m_tPrintMode = PRINTMODE::LOWER;
            m_pAnimator  = m_pEditPlayer->GetAnimator2();
        }
        else if (m_tPrintMode == PRINTMODE::LOWER)
        {
            m_tPrintMode = PRINTMODE::UPPER;
            m_pAnimator  = m_pEditPlayer->GetAnimator();
        }
    }

    if (m_tPrintMode == PRINTMODE::LOWER)
    {
        if (m_bAnimator2)
            SetCurFrm(m_pEditPlayer->GetAnimator2());
    }
    else
    {
        if (m_bAnimator1)
            SetCurFrm(m_pEditPlayer->GetAnimator());
    }

    if (IsTap(KEY::F5))
    {
        DeleteObject(LAYER::EDITUI);
        m_vecPrevUpperFrm.clear();
        m_vecWorkUpperFrm.clear();
        m_vecPrevLowerFrm.clear();
        m_vecWorkLowerFrm.clear();
        m_vecPrevNormalFrm.clear();
        m_vecWorkNormalFrm.clear();
        m_bAnimator1 = false;
        m_bAnimator2 = false;
        return;
    }

    if (IsTap(KEY::SPACE))
        CTimeMgr::GetInst()->SetTimeStop();
    if (IsTap(KEY::R))
        PlayLeft();
    if (IsTap(KEY::F))
        PlayRight();

    if (IsPressed(KEY::UP))
        m_pEidtAnim->SetPos(Vec2(m_pEidtAnim->GetPos().x, m_pEidtAnim->GetPos().y + 300.f * DT));
    if (IsPressed(KEY::DOWN))
        m_pEidtAnim->SetPos(Vec2(m_pEidtAnim->GetPos().x, m_pEidtAnim->GetPos().y - 300.f * DT));
    if (IsPressed(KEY::RIGHT))
        m_pEidtAnim->SetPos(Vec2(m_pEidtAnim->GetPos().x - 300.f * DT, m_pEidtAnim->GetPos().y));
    if (IsPressed(KEY::LEFT))
        m_pEidtAnim->SetPos(Vec2(m_pEidtAnim->GetPos().x + 300.f * DT, m_pEidtAnim->GetPos().y));
}

void CAnimEditor::render(const HDC _dc)
{
    CLevel::render(_dc);

    PrintMouseInfo(_dc);

    PrintMousePos(_dc);

    PrintInfo(_dc);

    if (m_tPrintMode == PRINTMODE::UPPER || m_tPrintMode == PRINTMODE::NORMAL)
    {
        if (m_bAnimator1)
            PrintEditAnimInfo(_dc, m_pEditPlayer->GetAnimator());
    }
    if (m_tPrintMode == PRINTMODE::LOWER)
    {
        if (m_bAnimator2)
            PrintEditAnimInfo(_dc, m_pEditPlayer->GetAnimator2());
    }
    {
        LPCTSTR UpDown = {};
        if (m_tPrintMode == PRINTMODE::UPPER)
            UpDown = L"EditState : UPPER";
        else if (m_tPrintMode == PRINTMODE::LOWER)
            UpDown = L"EditState : LOWER";
        else if (m_tPrintMode == PRINTMODE::NORMAL)
            UpDown = L"EditState : NORMAL";

        TextOut(_dc, 40, 20, UpDown, lstrlenW(UpDown));
    }
    {
        LPCTSTR PressMode = {};
        if (m_bPressMode == true)
            PressMode = L"PRESS : ON";
        else if (m_bPressMode == false)
            PressMode = L"PRESS : OFF";

        TextOut(_dc, 40, 40, PressMode, lstrlenW(PressMode));
    }
    {
        LPCTSTR OffsetPoress = {};
        if (m_bOffsetPress == true)
            OffsetPoress = L"OFFSETPRESS : ON";
        else if (m_bOffsetPress == false)
            OffsetPoress = L"OFFSETPRESS : OFF";

        TextOut(_dc, 40, 60, OffsetPoress, lstrlenW(OffsetPoress));
    }
}

void CAnimEditor::Exit()
{
    const HWND hWnd = CEngine::GetInst()->GetMainWnd();
    SetMenu(hWnd, nullptr);

    DeleteObject();
}

void CAnimEditor::Enter()
{
    if (m_hMenu == nullptr)
        m_hMenu = LoadMenu(nullptr, MAKEINTRESOURCEW(IDC_CLIENT));

    const HWND hWnd = CEngine::GetInst()->GetMainWnd();
    SetMenu(hWnd, m_hMenu);

    CEngine::GetInst()->ChangeWindowSize(static_cast<UINT>(m_vResolution.x), static_cast<UINT>(m_vResolution.y));

    init();
}

void CAnimEditor::CreateAnimation()
{
    tAnimFrm  frm  = {};
    const int Left = static_cast<int>(m_ptRealStart.x);
    const int Top  = static_cast<int>(m_ptRealStart.y);
    frm.vLeftTop   = Vec2(static_cast<float>(Left), static_cast<float>(Top));
    frm.vSize      = m_vecDragRectSize;
    frm.vOffset    = Vec2(0.f, 0.f);
    frm.fDuration  = 0.1f;

    if (m_tPrintMode == PRINTMODE::UPPER)
    {
        m_vecWorkUpperFrm.push_back(frm);
    }
    else if (m_tPrintMode == PRINTMODE::LOWER)
    {
        m_vecWorkLowerFrm.push_back(frm);
    }
    else if (m_tPrintMode == PRINTMODE::NORMAL)
    {
        m_vecWorkNormalFrm.push_back(frm);
    }
    else
    {
        assert(false);
    }
}

void CAnimEditor::SetCurFrm(CAnimator* _pAnimator)
{
    if (IsTap(KEY::Z))
        FirstFrm(_pAnimator);

    if (IsTap(KEY::X))
        PrevFrm(_pAnimator);

    if (IsTap(KEY::C))
        NextFrm(_pAnimator);


    if (m_bPressMode == true)
    {
        if (IsPressed(KEY::Q))
        {
            m_fKeyPressDelay += 0.002f;
            if (m_fKeyPressDelay > 0.1f)
            {
                GETCURFRM(_pAnimator);
                CurFrm.fDuration -= 0.01f;
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                m_fKeyPressDelay = 0.f;
            }
        }

        if (IsPressed(KEY::E))
        {
            m_fKeyPressDelay += 0.002f;
            if (m_fKeyPressDelay > 0.1f)
            {
                GETCURFRM(_pAnimator);
                CurFrm.fDuration += 0.01f;
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                m_fKeyPressDelay = 0.f;
            }
        }
    }
    else
    {
        if (IsTap(KEY::Q))
        {
            GETCURFRM(_pAnimator);
            CurFrm.fDuration -= 0.01f;
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
        if (IsTap(KEY::E))
        {
            GETCURFRM(_pAnimator);
            CurFrm.fDuration += 0.01f;
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
    }

    if (IsPressed(KEY::LSHIFT))
    {
        if (m_bPressMode == true)
        {
            if (m_bOffsetPress == false)
            {
                if (IsTap(KEY::W))
                {
                    GETALLFRM(_pAnimator);
                    for (size_t i = 0; i < AllFrm.size(); ++i)
                    {
                        AllFrm[i].vOffset -= Vec2(0.f, 1.f);
                        _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                    }
                }
                if (IsTap(KEY::S))
                {
                    GETALLFRM(_pAnimator);
                    for (size_t i = 0; i < AllFrm.size(); ++i)
                    {
                        AllFrm[i].vOffset += Vec2(0.f, 1.f);
                        _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                    }
                }
                if (IsTap(KEY::A))
                {
                    GETALLFRM(_pAnimator);
                    for (size_t i = 0; i < AllFrm.size(); ++i)
                    {
                        AllFrm[i].vOffset -= Vec2(1.f, 0.f);
                        _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                    }
                }
                if (IsTap(KEY::D))
                {
                    GETALLFRM(_pAnimator);
                    for (size_t i = 0; i < AllFrm.size(); ++i)
                    {
                        AllFrm[i].vOffset += Vec2(1.f, 0.f);
                        _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                    }
                }
            }
            else if (m_bOffsetPress == true)
            {
                if (IsPressed(KEY::W))
                {
                    m_fKeyPressDelay += 0.003f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETALLFRM(_pAnimator);
                        for (size_t i = 0; i < AllFrm.size(); ++i)
                        {
                            AllFrm[i].vOffset -= Vec2(0.f, 1.f);
                            _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                            m_fKeyPressDelay = 0.f;
                        }
                    }
                }
                if (IsPressed(KEY::S))
                {
                    m_fKeyPressDelay += 0.003f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETALLFRM(_pAnimator);
                        for (size_t i = 0; i < AllFrm.size(); ++i)
                        {
                            AllFrm[i].vOffset += Vec2(0.f, 1.f);
                            _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                            m_fKeyPressDelay = 0.f;
                        }
                    }
                }
                if (IsPressed(KEY::A))
                {
                    m_fKeyPressDelay += 0.003f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETALLFRM(_pAnimator);
                        for (size_t i = 0; i < AllFrm.size(); ++i)
                        {
                            AllFrm[i].vOffset -= Vec2(1.f, 0.f);
                            _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                            m_fKeyPressDelay = 0.f;
                        }
                    }
                }
                if (IsPressed(KEY::D))
                {
                    m_fKeyPressDelay += 0.003f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETALLFRM(_pAnimator);
                        for (size_t i = 0; i < AllFrm.size(); ++i)
                        {
                            AllFrm[i].vOffset += Vec2(1.f, 0.f);
                            _pAnimator->GetAnimation()->SetAllFrmInfo(AllFrm);
                            m_fKeyPressDelay = 0.f;
                        }
                    }
                }
            }
        }
        else if (m_bPressMode == false)
        {
            if (m_bOffsetPress == false)
            {
                if (IsTap(KEY::W))
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vOffset -= Vec2(0.f, 1.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                }
                if (IsTap(KEY::S))
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vOffset += Vec2(0.f, 1.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                }
                if (IsTap(KEY::A))
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vOffset -= Vec2(1.f, 0.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                }
                if (IsTap(KEY::D))
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vOffset += Vec2(1.f, 0.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                }
            }
            else if (m_bOffsetPress == true)
            {
                if (IsPressed(KEY::W))
                {
                    m_fKeyPressDelay += 0.002f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETCURFRM(_pAnimator);
                        CurFrm.vOffset -= Vec2(0.f, 1.f);
                        _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                        m_fKeyPressDelay = 0.f;
                    }
                }

                if (IsPressed(KEY::S))
                {
                    m_fKeyPressDelay += 0.002f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETCURFRM(_pAnimator);
                        CurFrm.vOffset += Vec2(0.f, 1.f);
                        _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                        m_fKeyPressDelay = 0.f;
                    }
                }

                if (IsPressed(KEY::A))
                {
                    m_fKeyPressDelay += 0.002f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETCURFRM(_pAnimator);
                        CurFrm.vOffset -= Vec2(1.f, 0.f);
                        _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                        m_fKeyPressDelay = 0.f;
                    }
                }

                if (IsPressed(KEY::D))
                {
                    m_fKeyPressDelay += 0.002f;
                    if (m_fKeyPressDelay > 0.1f)
                    {
                        GETCURFRM(_pAnimator);
                        CurFrm.vOffset += Vec2(1.f, 0.f);
                        _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                        m_fKeyPressDelay = 0.f;
                    }
                }
            }
        }
    }
    else if (IsPressed(KEY::LCTRL))
    {
        if (IsTap(KEY::W))
        {
            GETCURFRM(_pAnimator);
            CurFrm.vSize -= Vec2(0.f, 1.f);
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
        if (IsTap(KEY::S))
        {
            GETCURFRM(_pAnimator);
            CurFrm.vSize += Vec2(0.f, 1.f);
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
        if (IsTap(KEY::A))
        {
            GETCURFRM(_pAnimator);
            CurFrm.vSize -= Vec2(1.f, 0.f);
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
        if (IsTap(KEY::D))
        {
            GETCURFRM(_pAnimator);
            CurFrm.vSize += Vec2(1.f, 0.f);
            _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
        }
    }
    else
    {
        if (m_bPressMode == true)
        {
            if (IsPressed(KEY::W))
            {
                m_fKeyPressDelay += 0.002f;
                if (m_fKeyPressDelay > 0.1f)
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vLeftTop -= Vec2(0.f, 1.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                    m_fKeyPressDelay = 0.f;
                }
            }

            if (IsPressed(KEY::S))
            {
                m_fKeyPressDelay += 0.002f;
                if (m_fKeyPressDelay > 0.1f)
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vLeftTop += Vec2(0.f, 1.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                    m_fKeyPressDelay = 0.f;
                }
            }

            if (IsPressed(KEY::A))
            {
                m_fKeyPressDelay += 0.002f;
                if (m_fKeyPressDelay > 0.1f)
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vLeftTop -= Vec2(1.f, 0.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                    m_fKeyPressDelay = 0.f;
                }
            }

            if (IsPressed(KEY::D))
            {
                m_fKeyPressDelay += 0.002f;
                if (m_fKeyPressDelay > 0.1f)
                {
                    GETCURFRM(_pAnimator);
                    CurFrm.vLeftTop += Vec2(1.f, 0.f);
                    _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
                    m_fKeyPressDelay = 0.f;
                }
            }
        }
        else
        {
            if (IsTap(KEY::W))
            {
                GETCURFRM(_pAnimator);
                CurFrm.vLeftTop -= Vec2(0.f, 1.f);
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
            }

            if (IsTap(KEY::S))
            {
                GETCURFRM(_pAnimator);
                CurFrm.vLeftTop += Vec2(0.f, 1.f);
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
            }

            if (IsTap(KEY::A))
            {
                GETCURFRM(_pAnimator);
                CurFrm.vLeftTop -= Vec2(1.f, 0.f);
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
            }

            if (IsTap(KEY::D))
            {
                GETCURFRM(_pAnimator);
                CurFrm.vLeftTop += Vec2(1.f, 0.f);
                _pAnimator->GetAnimation()->SetCurFrmInfo(CurFrm);
            }
        }
    }
}

void CAnimEditor::TimeStop() const
{
    CTimeMgr::GetInst()->SetTimeStop();
}

void CAnimEditor::FirstFrm(CAnimator* _pAnimator) const
{
    _pAnimator->GetAnimation()->FirstFrm();
}

void CAnimEditor::PrevFrm(CAnimator* _pAnimator) const
{
    _pAnimator->GetAnimation()->PrevFrm();
}

void CAnimEditor::NextFrm(CAnimator* _pAnimator) const
{
    _pAnimator->GetAnimation()->NextFrm();
}

INT_PTR CALLBACK CreateAnimation(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        return TRUE;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDOK)
        {
            const int UpperLower = GetDlgItemInt(hDlg, IDC_EDIT1, nullptr, true);

            const float Left   = static_cast<float>(GetDlgItemInt(hDlg, IDC_EDIT2, nullptr, true));
            const float Top    = static_cast<float>(GetDlgItemInt(hDlg, IDC_EDIT3, nullptr, true));
            const Vec2  vLfTop = Vec2(Left, Top);

            const int  Right     = static_cast<int>(GetDlgItemInt(hDlg, IDC_EDIT4, nullptr, true));
            const int  Bottom    = static_cast<int>(GetDlgItemInt(hDlg, IDC_EDIT5, nullptr, true));
            const Vec2 vRtBottom = Vec2(static_cast<float>(Right), static_cast<float>(Bottom));

            const int iMaxFrm     = static_cast<int>(GetDlgItemInt(hDlg, IDC_EDIT6, nullptr, true));
            const int fDuration10 = static_cast<int>(GetDlgItemInt(hDlg, IDC_EDIT7, nullptr, true));

            if (UpperLower > 1 || UpperLower < 0)
            {
                if (!(Right && Bottom && iMaxFrm && fDuration10))
                {
                    MessageBox(nullptr, L"입력하지 않은 값이 있습니다", L"애니메이션 생성 실패", MB_OK);
                    return TRUE;
                }
            }
            CLevel*           pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
            const CAnimEditor pEditAnim;
            if (UpperLower == 0)
            {
                pCurLevel->DeleteObject(LAYER::EDITUI);
                pEditAnim.m_pEditPlayer->GetAnimator()->CreateAnimation(L"WorkNormalAnim", pEditAnim.m_pAtlas, vLfTop, vRtBottom, Vec2(0.f, 0.f), iMaxFrm, static_cast<float>(fDuration10 / 100));
                pEditAnim.m_pEditPlayer->GetAnimator()->Play(L"WorkNormalAnim", true);
                pEditAnim.m_pEditPlayer->GetAnimator()->GetAnimation()->SetMagni(4.f);
                pEditAnim.SaveUpperAnimation();
            }
            else if (UpperLower == 1)
            {
                pCurLevel->DeleteObject(LAYER::EDITUI);
                pEditAnim.m_pEditPlayer->GetAnimator2()->CreateAnimation(L"WorkNormalAnim", pEditAnim.m_pAtlas, vLfTop, vRtBottom, Vec2(0.f, 0.f), iMaxFrm, static_cast<float>(fDuration10 / 10));
                pEditAnim.m_pEditPlayer->GetAnimator2()->Play(L"WorkNormalAnim", true);
                pEditAnim.m_pEditPlayer->GetAnimator2()->GetAnimation()->SetMagni(4.f);
                pEditAnim.SaveLowerAnimation();
            }
            else
            {
                MessageBox(nullptr, L"애니메이션 생성 실패", L"애니메이션 생성 실패", MB_OK);
            }

            return TRUE;
        }

        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
    }
    break;
    default: ;
    }

    return FALSE;
}
