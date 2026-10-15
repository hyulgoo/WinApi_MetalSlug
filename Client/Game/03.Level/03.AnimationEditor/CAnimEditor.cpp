#include "pch.h"
#include "Game\03.Level\03.AnimationEditor\CAnimEditor.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\04.Object\UI\PanelUI\CEditorPanel.h"
#include "Game\04.Object\UI\EditAnim\EditPlayer\CEditPlayer.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include "resource.h"

#include <cmath>

namespace
{
    bool IsDown(const KEY _eKey)
    {
        return IsTap(_eKey) || IsPressed(_eKey);
    }

    // 재생 시간은 파일에 소수 둘째 자리까지 저장된다. 고칠 때마다 맞춰 두어 화면에 보이는 값과 저장되는 값을 같게
    float RoundDuration(const float _fDuration)
    {
        return max(0.f, roundf(_fDuration * 100.f) / 100.f);
    }
}

CAnimEditor::CAnimEditor()
{
    m_vResolution = CEngine::GetInst()->GetResolution();
}

void CAnimEditor::LoadData()
{
    CDataMgr* pData = CDataMgr::GetInst();

    const CDataSection& tView = pData->GetSection(L"editor", L"view");
    m_tData.fSheetMagni       = tView.GetFloat(L"sheet_magni");
    m_tData.iPanelWidthDiv    = tView.GetInt(L"panel_width_div");
    m_tData.fPreviewYRatio    = tView.GetFloat(L"preview_y_ratio");
    m_tData.fSheetKeep        = tView.GetFloat(L"sheet_keep");
    m_tData.fScrollSpeed      = tView.GetFloat(L"scroll_speed");
    m_tData.fScrollFast       = tView.GetFloat(L"scroll_fast");

    const CDataSection& tInput = pData->GetSection(L"editor", L"input");
    m_tData.fRepeatDelay       = tInput.GetFloat(L"repeat_delay");
    m_tData.fRepeatInterval    = tInput.GetFloat(L"repeat_interval");

    const CDataSection& tEdit = pData->GetSection(L"editor", L"edit");
    m_tData.fDefaultDuration  = tEdit.GetFloat(L"default_duration");
    m_tData.iMaxUndo          = static_cast<size_t>(max(0, tEdit.GetInt(L"max_undo")));

    const CDataSection& tGrid = pData->GetSection(L"editor", L"grid");
    m_tData.tGrid             = {};
    m_tData.tGrid.iWidth      = tGrid.GetInt(L"width");
    m_tData.tGrid.iHeight     = tGrid.GetInt(L"height");
    m_tData.tGrid.iCount      = tGrid.GetInt(L"count");
    m_tData.tGrid.iDuration   = tGrid.GetInt(L"duration");

    const CDataSection& tPanel  = pData->GetSection(L"editor", L"panel");
    m_tData.vButtonSize         = tPanel.GetVec2(L"button_size");
    m_tData.iButtonsPerRow      = tPanel.GetInt(L"buttons_per_row");
    m_tData.fButtonGap          = tPanel.GetFloat(L"button_gap");
    m_tData.fMargin             = tPanel.GetFloat(L"margin");
    m_tData.iInfoTop            = tPanel.GetInt(L"info_top");
    m_tData.iLineHeight         = tPanel.GetInt(L"line_height");
    m_tData.tInfoColor          = tPanel.GetColor(L"info_color");
    m_tData.tSheetTextColor     = tPanel.GetColor(L"sheet_text_color");
    m_tData.vFrameNumberOffset  = tPanel.GetVec2(L"frame_number_offset");
    m_tData.vDragTextOffset     = tPanel.GetVec2(L"drag_text_offset");
    m_tData.vMouseTextPos       = tPanel.GetVec2(L"mouse_text_pos");
}

CAnimEditor::~CAnimEditor()
{
    if (nullptr != m_hMenu)
        DestroyMenu(m_hMenu);
}

void CAnimEditor::Enter()
{
    LoadData();
    m_fPanelLeft = m_vResolution.x - m_vResolution.x / static_cast<float>(m_tData.iPanelWidthDiv);

    if (nullptr == m_hMenu)
        m_hMenu = LoadMenu(nullptr, MAKEINTRESOURCEW(IDC_CLIENT));
    SetMenu(CEngine::GetInst()->GetMainWnd(), m_hMenu);
    CEngine::GetInst()->ChangeWindowSize(static_cast<UINT>(m_vResolution.x), static_cast<UINT>(m_vResolution.y));
    CCamera::GetInst()->SetLook(m_vResolution / 2.f);

    // 들어올 때마다 처음 상태로 (이전 편집이 남지 않게)
    m_bDrag        = false;
    m_bPan         = false;
    m_bWaitRelease = false;
    m_mapRepeat.clear();
    m_bOffsetAll   = false;
    m_bHelp        = true;
    m_ePreview     = PREVIEW::NORMAL;
    m_bPreviewLeft = false;
    m_bPause       = false;
    m_tRef         = {};
    m_eRefPreview  = PREVIEW::NORMAL;
    m_fSheetMagni  = m_tData.fSheetMagni;

    // 미리보기는 패널 가운데 아래쪽 (기준선 위에 선다)
    m_pPreview = new CEditPlayer;
    m_pPreview->SetPos(Vec2(m_fPanelLeft + (m_vResolution.x - m_fPanelLeft) / 2.f, m_vResolution.y * m_tData.fPreviewYRatio));
    AddObject(m_pPreview, LAYER::EDITANIM);
    CreateUI();

    // 시트 목록은 데이터(.anim 파일들이 쓰는 아틀라스)에서 모은다. 처음에는 플레이어 시트에 빈 애니메이션
    // (예전에는 편집할 시트를 두 파일의 주석으로 고르고 다시 빌드했다)
    ScanSheets();
    const int iPlayer = FindSheet(L"Player");
    SelectSheet(iPlayer >= 0 ? iPlayer : 0);
}

void CAnimEditor::Exit()
{
    SetMenu(CEngine::GetInst()->GetMainWnd(), nullptr);

    DeleteAllObjects();

    m_pPreview = nullptr;
    m_pPanel   = nullptr;
    m_pSheet   = nullptr;
    m_tDoc     = {};
    m_tRef     = {};
    m_vecUndo.clear();
}

void CAnimEditor::tick()
{
    CLevel::tick();   // 패널·버튼·미리보기

    UpdateCommandKeys();
    UpdateSheetView();
    UpdateMouse();
    UpdateEditKeys();

    // 멈춘 미리보기는 고르고 있는 프레임을 보여 준다
    if (m_bPause && nullptr != m_pPreview)
    {
        if (CAnimation* pDoc = m_pPreview->GetAnimator()->GetAnimation())
            pDoc->SetFrmNum(m_iCurFrm);
        if (CAnimation* pRef = m_pPreview->GetAnimator2()->GetAnimation())
            pRef->SetFrmNum(m_iCurFrm);
    }
}

// ---------------------------------------------------------------- 입력

void CAnimEditor::UpdateCommandKeys()
{
    if (IsTap(KEY::F4))
        GoTitle();
    if (IsTap(KEY::F5))
        NewDoc();
    if (IsTap(KEY::F1))
        m_bHelp = !m_bHelp;
    if (IsTap(KEY::F2))
        m_bOffsetAll = !m_bOffsetAll;

    // 미리보기: TAB 출력 방식(전신 → 상체 → 하체), R/F 방향, SPACE 멈춤
    // (예전 TAB은 상체·하체만 오가서 전신 모드에 갈 수 없었다)
    bool bPreview = false;
    if (IsTap(KEY::TAB))
    {
        if (PREVIEW::NORMAL == m_ePreview)
            m_ePreview = PREVIEW::UPPER;
        else if (PREVIEW::UPPER == m_ePreview)
            m_ePreview = PREVIEW::LOWER;
        else
            m_ePreview = PREVIEW::NORMAL;
        bPreview = true;
    }
    if (IsTap(KEY::R))
    {
        m_bPreviewLeft = true;
        bPreview       = true;
    }
    if (IsTap(KEY::F))
    {
        m_bPreviewLeft = false;
        bPreview       = true;
    }
    if (IsTap(KEY::SPACE))
    {
        m_bPause = !m_bPause;
        bPreview = true;
    }
    if (bPreview)
        SyncPreview();
}

void CAnimEditor::UpdateSheetView()
{
    // 화살표: 보는 곳을 옮긴다 (Shift면 빠르게)
    const float fMove = m_tData.fScrollSpeed * (IsDown(KEY::LSHIFT) ? m_tData.fScrollFast : 1.f) * DT;
    if (IsDown(KEY::LEFT))
        m_vSheetPos.x += fMove;
    if (IsDown(KEY::RIGHT))
        m_vSheetPos.x -= fMove;
    if (IsDown(KEY::UP))
        m_vSheetPos.y += fMove;
    if (IsDown(KEY::DOWN))
        m_vSheetPos.y -= fMove;

    // 오른쪽 버튼으로 시트를 잡아 끈다
    const Vec2 vMouse = MOUSE_POS;
    if (IsTap(KEY::RBTN) && IsOnSheetArea(vMouse))
    {
        m_bPan      = true;
        m_vPanMouse = vMouse;
        m_vPanSheet = m_vSheetPos;
    }
    if (m_bPan)
    {
        if (IsDown(KEY::RBTN))
            m_vSheetPos = m_vPanSheet + (vMouse - m_vPanMouse);
        else
            m_bPan = false;
    }

    // 1~4: 배율
    const KEY arrZoomKey[] = { KEY::_1, KEY::_2, KEY::_3, KEY::_4 };
    for (int i = 0; i < 4; ++i)
    {
        if (IsTap(arrZoomKey[i]))
            SetSheetMagni(static_cast<float>(i + 1));
    }

    ClampSheetPos();
}

void CAnimEditor::UpdateMouse()
{
    const Vec2 vMouse = MOUSE_POS;

    // 대화상자를 닫은 직후 아직 눌려 있는 버튼(파일 더블클릭 등)으로는 드래그를 시작하지 않는다
    if (m_bWaitRelease)
    {
        if (IsDown(KEY::LBTN))
            return;
        m_bWaitRelease = false;
    }

    // 시트 그림 위에서 누른 것만 드래그로 본다 (시트 밖 빈 곳에서 시작하면 가장자리로 잘린 이상한 프레임이 생긴다)
    if (IsTap(KEY::LBTN) && IsOnSheetArea(vMouse) && IsOnSheetImage(vMouse))
    {
        m_bDrag      = true;
        m_vDragStart = ToSheetPixel(vMouse);
        m_vDragEnd   = m_vDragStart;
    }
    if (!m_bDrag)
        return;

    m_vDragEnd = ToSheetPixel(vMouse);
    if (IsDown(KEY::LBTN))
        return;

    // 뗐다: 끌었으면 그 사각형으로 프레임을 더하고, 그냥 눌렀다 뗐으면 그 자리의 프레임을 고른다.
    // 거꾸로 끌어도 왼쪽 위·크기로 맞춘다 (예전에는 끝 − 시작을 그대로 크기로 써 음수 크기가 저장됐다, MS-R23)
    m_bDrag = false;
    const Vec2 vLeftTop(min(m_vDragStart.x, m_vDragEnd.x), min(m_vDragStart.y, m_vDragEnd.y));
    const Vec2 vSize(fabsf(m_vDragEnd.x - m_vDragStart.x), fabsf(m_vDragEnd.y - m_vDragStart.y));
    if (vSize.x >= 1.f && vSize.y >= 1.f)
        AddFrame(vLeftTop, vSize);
    else
        SelectFrameAt(m_vDragStart);
}

void CAnimEditor::UpdateEditKeys()
{
    const bool bCtrl  = IsDown(KEY::LCTRL);
    const bool bShift = IsDown(KEY::LSHIFT);

    // 반복 시간은 키마다 잰다 (프레임이 없어도 매 tick 불러 시간을 이어 간다)
    const int iPrev    = KeyRepeat(KEY::X);
    const int iNext    = KeyRepeat(KEY::C);
    const int iShorter = KeyRepeat(KEY::Q);
    const int iLonger  = KeyRepeat(KEY::E);
    const int iLeft    = KeyRepeat(KEY::A);
    const int iRight   = KeyRepeat(KEY::D);
    const int iUp      = KeyRepeat(KEY::W);
    const int iDown    = KeyRepeat(KEY::S);

    if (bCtrl && IsTap(KEY::Z))
    {
        Undo();
        return;
    }
    if (m_tDoc.vecFrm.empty())
        return;

    // 프레임 고르기: Z 처음, X/C 이전·다음 (고른 프레임이 화면 밖이면 시트를 옮긴다)
    const bool bFirst = !bCtrl && IsTap(KEY::Z);
    if (bFirst)
        SelectFrame(0);
    if (0 != iPrev || 0 != iNext)
        SelectFrame(m_iCurFrm + iNext - iPrev);
    if (bFirst || 0 != iPrev || 0 != iNext)
        ScrollToFrame(m_iCurFrm, true);

    // 삭제·순서 (예전에는 프레임을 지울 수 없어 재생 시간을 0으로 낮춰 숨겼다)
    if (IsTap(KEY::DEL))
    {
        DeleteFrame();
        return;
    }
    if (IsTap(KEY::J))
        MoveFrame(-1);
    if (IsTap(KEY::L))
        MoveFrame(1);

    // 재생 시간: Q 줄이기, E 늘리기 (0.01초씩). 누르고 있는 동안의 반복은 되돌리기 한 번으로 묶는다
    if (0 != iLonger - iShorter)
    {
        if (IsTap(KEY::Q) || IsTap(KEY::E))
            PushUndo();
        tAnimFrm& frm = m_tDoc.vecFrm[m_iCurFrm];
        frm.fDuration = RoundDuration(frm.fDuration + 0.01f * static_cast<float>(iLonger - iShorter));
        OnDocChanged();
    }

    // WASD: 왼쪽 위 / Shift+WASD: 오프셋 (F2면 모든 프레임) / Ctrl+WASD: 크기. 1px씩
    const Vec2 vStep(static_cast<float>(iRight - iLeft), static_cast<float>(iDown - iUp));
    if (0.f != vStep.x || 0.f != vStep.y)
    {
        if (IsTap(KEY::W) || IsTap(KEY::A) || IsTap(KEY::S) || IsTap(KEY::D))
            PushUndo();

        tAnimFrm& frm = m_tDoc.vecFrm[m_iCurFrm];
        if (bShift)
        {
            if (m_bOffsetAll)
            {
                for (tAnimFrm& tFrm : m_tDoc.vecFrm)
                    tFrm.vOffset += vStep;
            }
            else
            {
                frm.vOffset += vStep;
            }
        }
        else if (bCtrl)
        {
            frm.vSize   += vStep;
            frm.vSize.x  = max(1.f, frm.vSize.x);
            frm.vSize.y  = max(1.f, frm.vSize.y);
        }
        else
        {
            frm.vLeftTop   += vStep;
            frm.vLeftTop.x  = max(0.f, frm.vLeftTop.x);
            frm.vLeftTop.y  = max(0.f, frm.vLeftTop.y);
        }
        OnDocChanged();
    }
}

int CAnimEditor::KeyRepeat(const KEY _eKey)
{
    float& fTime = m_mapRepeat[_eKey];
    if (IsTap(_eKey))
    {
        fTime = m_tData.fRepeatInterval - m_tData.fRepeatDelay;   // repeat_delay 뒤에 첫 반복
        return 1;
    }
    if (!IsPressed(_eKey))
        return 0;

    // DT로 잰다 (예전에는 프레임마다 0.002씩 더해 FPS에 따라 반복 속도가 달랐고, F2 모드에서만 반복했다)
    fTime += DT;
    int iCount = 0;
    while (fTime >= m_tData.fRepeatInterval)
    {
        fTime -= m_tData.fRepeatInterval;
        ++iCount;
    }
    return iCount;
}

Vec2 CAnimEditor::SheetOrigin() const
{
    // 그리기와 좌표 계산이 같은 정수 위치를 쓴다
    return Vec2(floorf(m_vSheetPos.x), floorf(m_vSheetPos.y));
}

Vec2 CAnimEditor::ToSheetPixel(const Vec2& _vScreen) const
{
    // 시트를 그린 위치를 빼고 배율로 나눈다
    // (예전에는 GetRealPos(마우스) / 배율이라, 화살표로 시트를 옮긴 만큼 좌표가 틀어졌다, MS-R24)
    const Vec2 vOrigin = SheetOrigin();
    Vec2       vPixel(floorf((_vScreen.x - vOrigin.x) / m_fSheetMagni), floorf((_vScreen.y - vOrigin.y) / m_fSheetMagni));
    if (nullptr != m_pSheet)
    {
        const float fWidth  = static_cast<float>(m_pSheet->Width());
        const float fHeight = static_cast<float>(m_pSheet->Height());
        vPixel.x            = max(0.f, min(vPixel.x, fWidth));
        vPixel.y            = max(0.f, min(vPixel.y, fHeight));
    }
    return vPixel;
}

bool CAnimEditor::IsOnSheetImage(const Vec2& _vScreen) const
{
    if (nullptr == m_pSheet)
        return false;
    const Vec2 vOrigin = SheetOrigin();
    return vOrigin.x <= _vScreen.x && _vScreen.x < vOrigin.x + static_cast<float>(m_pSheet->Width()) * m_fSheetMagni
        && vOrigin.y <= _vScreen.y && _vScreen.y < vOrigin.y + static_cast<float>(m_pSheet->Height()) * m_fSheetMagni;
}

bool CAnimEditor::IsOnSheetArea(const Vec2& _vScreen) const
{
    // 패널(버튼) 위는 시트 입력이 아니다 (예전에는 버튼을 누르는 클릭이 시트 드래그로도 처리돼 크기 0 프레임이 생겼다, MS-R23)
    if (nullptr != m_pPanel && m_pPanel->IsMouseOn())
        return false;
    return 0.f <= _vScreen.x && _vScreen.x < m_fPanelLeft && 0.f <= _vScreen.y && _vScreen.y < m_vResolution.y;
}

void CAnimEditor::SetSheetMagni(const float _fMagni)
{
    // 마우스 아래 픽셀이 그 자리에 남게 (마우스가 시트 밖이면 편집 영역 가운데)
    Vec2 vAnchor = MOUSE_POS;
    if (!IsOnSheetArea(vAnchor))
        vAnchor = Vec2(m_fPanelLeft / 2.f, m_vResolution.y / 2.f);

    const Vec2 vPixel = (vAnchor - m_vSheetPos) / m_fSheetMagni;
    m_fSheetMagni     = _fMagni;
    m_vSheetPos       = vAnchor - vPixel * m_fSheetMagni;
    ClampSheetPos();
}

void CAnimEditor::ClampSheetPos()
{
    if (nullptr == m_pSheet)
        return;

    // 시트가 편집 영역 밖으로 다 나가지 않게 한다
    const float fWidth  = static_cast<float>(m_pSheet->Width()) * m_fSheetMagni;
    const float fHeight = static_cast<float>(m_pSheet->Height()) * m_fSheetMagni;
    const float fKeep   = m_tData.fSheetKeep;
    m_vSheetPos.x       = max(fKeep - fWidth, min(m_vSheetPos.x, m_fPanelLeft - fKeep));
    m_vSheetPos.y       = max(fKeep - fHeight, min(m_vSheetPos.y, m_vResolution.y - fKeep));
}

void CAnimEditor::ScrollToFrame(const int _iFrm, const bool _bOnlyIfHidden)
{
    if (_iFrm < 0 || _iFrm >= static_cast<int>(m_tDoc.vecFrm.size()))
        return;

    const tAnimFrm& frm = m_tDoc.vecFrm[_iFrm];
    if (_bOnlyIfHidden)
    {
        const Vec2 vLeftTop     = SheetOrigin() + frm.vLeftTop * m_fSheetMagni;
        const Vec2 vRightBottom = vLeftTop + frm.vSize * m_fSheetMagni;
        if (vRightBottom.x > 0.f && vLeftTop.x < m_fPanelLeft && vRightBottom.y > 0.f && vLeftTop.y < m_vResolution.y)
            return;
    }

    // 프레임 가운데를 편집 영역 가운데로. 시트가 편집 영역보다 크면 시트 가장자리 너머(빈 곳)는 보이지 않게 한다
    const Vec2 vCenter = (frm.vLeftTop + frm.vSize / 2.f) * m_fSheetMagni;
    m_vSheetPos        = Vec2(m_fPanelLeft / 2.f, m_vResolution.y / 2.f) - vCenter;
    if (nullptr != m_pSheet)
    {
        const float fWidth  = static_cast<float>(m_pSheet->Width()) * m_fSheetMagni;
        const float fHeight = static_cast<float>(m_pSheet->Height()) * m_fSheetMagni;
        m_vSheetPos.x       = fWidth > m_fPanelLeft ? max(m_fPanelLeft - fWidth, min(m_vSheetPos.x, 0.f)) : 0.f;
        m_vSheetPos.y       = fHeight > m_vResolution.y ? max(m_vResolution.y - fHeight, min(m_vSheetPos.y, 0.f)) : 0.f;
    }
    ClampSheetPos();
}

// ---------------------------------------------------------------- 문서

void CAnimEditor::SetDoc(const tAnimFile& _tDoc, CTexture* _pSheet)
{
    m_tDoc    = _tDoc;
    m_pSheet  = _pSheet;
    m_iCurFrm = 0;
    m_vecUndo.clear();
    m_iSerial      = ++m_iLastSerial;
    m_iSavedSerial = m_iSerial;
    m_bDrag        = false;
    ClampSheetPos();
    SyncPreview();
}

void CAnimEditor::SelectSheet(const int _iSheet)
{
    // 시트를 바꾸면 새 애니메이션을 시작한다 (프레임 좌표는 시트마다 뜻이 다르다)
    tAnimFile tNew;
    CTexture* pSheet = nullptr;
    if (0 <= _iSheet && _iSheet < static_cast<int>(m_vecSheet.size()))
    {
        tNew.strAtlasKey  = m_vecSheet[_iSheet].strKey;
        tNew.strAtlasPath = m_vecSheet[_iSheet].strPath;
        pSheet            = CResMgr::GetInst()->LoadTexture(tNew.strAtlasKey, tNew.strAtlasPath);
    }
    m_vSheetPos = Vec2(0.f, 0.f);
    m_strFilePath.clear();
    SetDoc(tNew, pSheet);
}

bool CAnimEditor::ConfirmDiscard() const
{
    return !IsDirty() || AskYesNo(L"애니메이션 에디터", L"저장하지 않은 변경이 있습니다. 버리고 계속할까요?");
}

void CAnimEditor::PushUndo()
{
    m_vecUndo.push_back(tUndo{ m_tDoc.vecFrm, m_iCurFrm, m_iSerial });
    if (m_vecUndo.size() > m_tData.iMaxUndo)
        m_vecUndo.erase(m_vecUndo.begin());
}

void CAnimEditor::OnDocChanged()
{
    m_iSerial = ++m_iLastSerial;
    SyncPreview();
}

tAnimFrm* CAnimEditor::CurFrm()
{
    if (m_tDoc.vecFrm.empty())
        return nullptr;
    return &m_tDoc.vecFrm[m_iCurFrm];
}

void CAnimEditor::SelectFrame(const int _iFrm)
{
    if (m_tDoc.vecFrm.empty())
    {
        m_iCurFrm = 0;
        return;
    }
    m_iCurFrm = max(0, min(_iFrm, static_cast<int>(m_tDoc.vecFrm.size()) - 1));
}

void CAnimEditor::SelectFrameAt(const Vec2& _vPixel)
{
    // 겹쳐 있으면 위에 그린(나중) 프레임
    for (int i = static_cast<int>(m_tDoc.vecFrm.size()) - 1; i >= 0; --i)
    {
        const tAnimFrm& frm = m_tDoc.vecFrm[i];
        if (frm.vLeftTop.x <= _vPixel.x && _vPixel.x < frm.vLeftTop.x + frm.vSize.x
            && frm.vLeftTop.y <= _vPixel.y && _vPixel.y < frm.vLeftTop.y + frm.vSize.y)
        {
            SelectFrame(i);
            return;
        }
    }
}

void CAnimEditor::AddFrame(const Vec2& _vLeftTop, const Vec2& _vSize)
{
    PushUndo();

    // 오프셋과 재생 시간은 고르고 있던 프레임을 따른다 (이어지는 프레임은 대개 비슷하다)
    tAnimFrm frm  = {};
    frm.vLeftTop  = _vLeftTop;
    frm.vSize     = _vSize;
    frm.fDuration = m_tData.fDefaultDuration;
    if (const tAnimFrm* pCur = CurFrm())
    {
        frm.vOffset   = pCur->vOffset;
        frm.fDuration = pCur->fDuration;
    }
    m_tDoc.vecFrm.push_back(frm);
    m_iCurFrm = static_cast<int>(m_tDoc.vecFrm.size()) - 1;
    OnDocChanged();
}

void CAnimEditor::DeleteFrame()
{
    if (m_tDoc.vecFrm.empty())
        return;

    PushUndo();
    m_tDoc.vecFrm.erase(m_tDoc.vecFrm.begin() + m_iCurFrm);
    SelectFrame(m_iCurFrm);
    OnDocChanged();
}

void CAnimEditor::MoveFrame(const int _iDir)
{
    const int iTo = m_iCurFrm + _iDir;
    if (iTo < 0 || iTo >= static_cast<int>(m_tDoc.vecFrm.size()))
        return;

    PushUndo();
    std::swap(m_tDoc.vecFrm[m_iCurFrm], m_tDoc.vecFrm[iTo]);
    m_iCurFrm = iTo;
    OnDocChanged();
}

void CAnimEditor::Undo()
{
    if (m_vecUndo.empty())
        return;

    tUndo& tPrev   = m_vecUndo.back();
    m_tDoc.vecFrm  = std::move(tPrev.vecFrm);
    m_iCurFrm      = tPrev.iCurFrm;
    m_iSerial      = tPrev.iSerial;
    m_vecUndo.pop_back();
    SelectFrame(m_iCurFrm);
    SyncPreview();
}

void CAnimEditor::NewDoc()
{
    if (!ConfirmDiscard())
        return;

    // 지금 시트에 빈 애니메이션
    tAnimFile tNew;
    tNew.strAtlasKey  = m_tDoc.strAtlasKey;
    tNew.strAtlasPath = m_tDoc.strAtlasPath;
    m_strFilePath.clear();
    SetDoc(tNew, m_pSheet);
}

void CAnimEditor::GoTitle()
{
    if (ConfirmDiscard())
        ChangeLevel(LEVEL_TYPE::START);
}
