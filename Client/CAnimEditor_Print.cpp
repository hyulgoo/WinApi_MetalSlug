#include "pch.h"
#include "CAnimEditor.h"
#include "CAnimation.h"
#include "CAnimator.h"

#include "CEngine.h"
#include "CEditorButton.h"
#include "CEditorPanel.h"
#include "CTexture.h"

#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CCamera.h"

#include "CEditAnim.h"
#include "CPlayer.h"
#include "CMouseArea.h"
#include "CEditPlayer.h"

#include "CLevelMgr.h"

void CAnimEditor::PrintCreateAnimation()
{
	if (IsTap(KEY::F1))
	{
		if (m_tPrintMode == PRINTMODE::UPPER)
		{
			vector<CObj*> vector = GetLayer(LAYER::EDITUI);
			if (vector.size() > 0)
			{
				if (m_pEditPlayer->GetAnimator()->GetAnimation() != nullptr)
				{
					if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"경고", MB_YESNO) == IDNO)
						return;
				}
				DeleteObject(LAYER::EDITUI);

				if (m_vecWorkUpperFrm.size() > 0)
				{
					assert(m_iCreatFrmCount == m_vecWorkUpperFrm.size());
					m_pEditPlayer->GetAnimator()->CreateEditUpperAnimation(m_vecWorkUpperFrm, m_pAtlas);
					m_pEditPlayer->GetAnimator()->Play(L"WorkAnim", true);
					m_pEditPlayer->GetAnimator()->GetAnimation()->SetUpper();
					m_bAnimator1 = true;
					m_vecPrevUpperFrm = m_vecWorkUpperFrm;
					m_iCreatFrmCount = 0;
					m_vecWorkUpperFrm.clear();
				}
			}
		}

		else if(m_tPrintMode == PRINTMODE::LOWER)
		{
			vector<CObj*> vector = GetLayer(LAYER::EDITUI);
			if (vector.size() > 0)
			{
				if (m_pEditPlayer->GetAnimator2()->GetAnimation() != nullptr)
				{
					if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"경고", MB_YESNO) == IDNO)
						return;
				}
				DeleteObject(LAYER::EDITUI);
				if (m_vecWorkLowerFrm.size() > 0)
				{
					assert(m_iCreatFrmCount == m_vecWorkLowerFrm.size());
					m_pEditPlayer->GetAnimator2()->CreateEditLowerAnimation(m_vecWorkLowerFrm, m_pAtlas);
					m_pEditPlayer->GetAnimator2()->Play(L"WorkAnim1", true);
					m_pEditPlayer->GetAnimator2()->GetAnimation()->SetLower();
					m_bAnimator2 = true;
					m_vecPrevUpperFrm = m_vecWorkLowerFrm;
					m_iCreatFrmCount = 0;
					m_vecWorkLowerFrm.clear();
				}
			}

		}
		else if(m_tPrintMode == PRINTMODE::NORMAL)
		{
			vector<CObj*> vector = GetLayer(LAYER::EDITUI);
			if (vector.size() > 0)
			{
				if (m_pEditPlayer->GetAnimator()->GetAnimation() != nullptr)
				{
					if (MessageBox(nullptr, L"작업중인 Animation이 삭제됩니다.", L"경고", MB_YESNO) == IDNO)
						return;
				}
				DeleteObject(LAYER::EDITUI);
				if (m_vecWorkNormalFrm.size() > 0)
				{
					assert(m_iCreatFrmCount == m_vecWorkNormalFrm.size());
					m_pEditPlayer->GetAnimator()->CreateEditUpperAnimation(m_vecWorkNormalFrm, m_pAtlas);
					m_pEditPlayer->GetAnimator()->Play(L"WorkAnim", true);
					m_bAnimator1 = true;
					m_vecPrevNormalFrm = m_vecWorkNormalFrm;
					m_iCreatFrmCount = 0;
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
		m_bIsStarted = true;
		Vec2 MousetapPos = MOUSE_POS;
		Vec2 RealtapPos = CCamera::GetInst()->GetRealPos(MousetapPos) / m_fmagni;
		m_ptRealStart.x = (int)(RealtapPos.x);
		m_ptRealStart.y = (int)(RealtapPos.y);
		m_ptStart.x = (int)(MousetapPos.x);
		m_ptStart.y = (int)(MousetapPos.y);
	}

	if (m_bIsStarted == true)
	{
		if (IsPressed(KEY::LBTN))
		{
			Vec2 MousePressPos = MOUSE_POS;
			Vec2 RealPressPos = CCamera::GetInst()->GetRealPos(MousePressPos) / m_fmagni;
			m_ptRealEnd.x = (int)(RealPressPos.x);
			m_ptRealEnd.y = (int)(RealPressPos.y);

			m_bIsPressed = true;
			m_ptEnd.x = (int)(MousePressPos.x);
			m_ptEnd.y = (int)(MousePressPos.y);
			m_vecDragRectSize = Vec2((float)(m_ptRealEnd.x - m_ptRealStart.x), (float)(m_ptRealEnd.y - m_ptRealStart.y));
		}
		if (IsRelease(KEY::LBTN))
		{
			Vec2 MouseReleasePos = MOUSE_POS;
			Vec2 RealReleasePos = CCamera::GetInst()->GetRealPos(MouseReleasePos) / m_fmagni;
			m_ptRealEnd.x = (int)(RealReleasePos.x);
			m_ptRealEnd.y = (int)(RealReleasePos.y);
			m_ptEnd.x = (int)(MouseReleasePos.x);
			m_ptEnd.y = (int)(MouseReleasePos.y);
			m_vecDragRectSize = Vec2((float)(m_ptRealEnd.x - m_ptRealStart.x), (float)(m_ptRealEnd.y - m_ptRealStart.y));

			CMouseArea* DragArea = new CMouseArea;
			DragArea->SetStartEnd(m_ptRealStart, m_ptRealEnd);
			DragArea->SetMagni(m_fmagni);
			Vec2 vPos = Vec2(m_ptRealStart.x - m_ptRealEnd.x / 2.f, m_ptRealStart.y - m_ptRealEnd.y / 2.f);
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

	Vec2 PanelScale = m_pEditPanel->GetScale();

	CEditorButton* pSaveUpperAnim = new CEditorButton;
	pSaveUpperAnim->SetPos(Vec2(PanelScale.x - 195, 10.f));
	pSaveUpperAnim->SetScale(Vec2(90.f, 30.f));
	pSaveUpperAnim->SetButtonTex(L"SaveUpper");
	pSaveUpperAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::SaveUpperAnimation);

	CEditorButton* pLoadUpperAnim = pSaveUpperAnim->Clone();
	pLoadUpperAnim->SetPos(Vec2(PanelScale.x - 95, 10.f));
	pLoadUpperAnim->SetButtonTex(L"LoadUpper");
	pLoadUpperAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::LoadUpperAnimation);

	CEditorButton* pSaveLowerAnim = pLoadUpperAnim->Clone();
	pSaveLowerAnim->SetPos(Vec2(PanelScale.x - 195, 50.f));
	pSaveLowerAnim->SetButtonTex(L"SaveLower");
	pSaveLowerAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::SaveLowerAnimation);

	CEditorButton* pLoadLowerAnim = pSaveLowerAnim->Clone();
	pLoadLowerAnim->SetPos(Vec2(PanelScale.x - 95, 50.f));
	pLoadLowerAnim->SetButtonTex(L"LoadLower");
	pLoadLowerAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::LoadLowerAnimation);

	CEditorButton* pSaveNormalAnim = pLoadLowerAnim->Clone();
	pSaveNormalAnim->SetPos(Vec2(PanelScale.x - 195, 90.f));
	pSaveNormalAnim->SetButtonTex(L"SaveNormal");
	pSaveNormalAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::SaveNormalAnimation);

	CEditorButton* pLoadNormalAnim = pSaveNormalAnim->Clone();
	pLoadNormalAnim->SetPos(Vec2(PanelScale.x - 95, 90.f));
	pLoadNormalAnim->SetButtonTex(L"LoadNormal");
	pLoadNormalAnim->SetDelegate(this, (DELEGATE)&CAnimEditor::LoadNormalAnimation);

	m_pEditPanel->AddChildUI(pSaveUpperAnim);
	m_pEditPanel->AddChildUI(pLoadUpperAnim);
	m_pEditPanel->AddChildUI(pSaveLowerAnim);
	m_pEditPanel->AddChildUI(pLoadLowerAnim);
	m_pEditPanel->AddChildUI(pSaveNormalAnim);
	m_pEditPanel->AddChildUI(pLoadNormalAnim);
	AddObject(m_pEditPanel, LAYER::UI);
}

void CAnimEditor::PrintMouseInfo(HDC _dc)
{
	// 드래그가 시작됐다면 필요한 좌표들을 출력해줌.
	if (m_bIsStarted == true)
	{
		HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HPEN hPen = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);

		// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
		HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
		HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);
		Rectangle(CEngine::GetInst()->GetMemTexDC()
			, (int)(m_ptStart.x)
			, (int)(m_ptStart.y)
			, (int)(m_ptEnd.x)
			, (int)(m_ptEnd.y));

		SelectObject(_dc, hOriginPen);
		SelectObject(_dc, hOriginBrush);

		SetTextAlign(_dc, TA_LEFT);

		TCHAR StartmousePos[128] = {};
		wsprintf(StartmousePos, TEXT("Letf : %d / Top : %d"), (int)m_ptRealStart.x, (int)m_ptRealStart.y);
		TCHAR EndmousePos[128] = {};
		wsprintf(EndmousePos, TEXT("Right : %d / Bottom : %d"), (int)m_ptRealEnd.x, (int)m_ptRealEnd.y);
		TCHAR RectSize[128] = {};
		wsprintf(RectSize, TEXT("Width : %d / Height : %d"), (UINT)(m_vecDragRectSize.x), (UINT)(m_vecDragRectSize.y));

		TextOut(_dc, 40, 60, StartmousePos, lstrlenW(StartmousePos));
		TextOut(_dc, 40, 80, EndmousePos, lstrlenW(EndmousePos));
		TextOut(_dc, 40, 100, RectSize, lstrlenW(RectSize));
	}
}
void CAnimEditor::PrintInfo(HDC _dc)
{
	SetTextAlign(_dc, TA_LEFT);
	LPCTSTR Explan = L"Z : FirstFrm  //  X, C : Prev, NextFrm ";
	LPCTSTR Explan1 = L"Q, E : Duration ";
	LPCTSTR Explan2 = L"WASD : LeftTop  //  WASD + LSHIFT : Offset ";
	LPCTSTR Explan3 = L"WASD + LCTRL : Size";
	LPCTSTR Explan4 = L"SPACE : PLAY / STOP";

	TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), (int)m_vResolution.y - 100, Explan, lstrlenW(Explan));
	TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), (int)m_vResolution.y - 80, Explan1, lstrlenW(Explan1));
	TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), (int)m_vResolution.y - 60, Explan2, lstrlenW(Explan2));
	TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), (int)m_vResolution.y - 40, Explan3, lstrlenW(Explan3));
	TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), (int)m_vResolution.y - 20, Explan4, lstrlenW(Explan4));
}

void CAnimEditor::PrintEditAnimInfo(HDC _dc, CAnimator* pAnimator)
{
		// 현재 출력 중인 애니메이션 정보 출력
		LPCTSTR AnimName = pAnimator->GetAnimation()->GetName().c_str();
		LPCTSTR Explan = L"z : 처음Frm  x, c : 이전, 다음 Frm ";
		vector<tAnimFrm> FrmInfo = pAnimator->GetAnimation()->GetFrmInfo();
		tAnimFrm CurFrm = pAnimator->GetAnimation()->GetCurFrm();

		TCHAR Frm[32] = {};
		wsprintf(Frm, TEXT("FrmCount : %d"), (int)(FrmInfo.size()));

		TCHAR CurFrmNum[32] = {};
		wsprintf(CurFrmNum, TEXT("CurFrm : %d"), pAnimator->GetAnimation()->GetCurFrmNumber() + 1);

		TCHAR LeftTop[32] = {};
		wsprintf(LeftTop, TEXT("LeftTop : x.%d,  y.%d"), (int)(CurFrm.vLeftTop.x), (int)(CurFrm.vLeftTop.y));

		TCHAR Size[32] = {};
		wsprintf(Size, TEXT("size : x.%d, y.%d "), (int)(CurFrm.vSize.x), (int)(CurFrm.vSize.y));

		TCHAR Offset[32] = {};
		wsprintf(Offset, TEXT("Offset : x.%d, y.%d"), (int)(CurFrm.vOffset.x), (int)(CurFrm.vOffset.y));

		TCHAR Duration[32] = {};
		wsprintf(Duration, TEXT("Duration : %d"), (int)(CurFrm.fDuration * 100));

		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 10, AnimName, lstrlenW(AnimName));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 30, Frm, lstrlenW(Frm));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 50, CurFrmNum, lstrlenW(CurFrmNum));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 70, LeftTop, lstrlenW(LeftTop));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 90, Size, lstrlenW(Size));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 110, Offset, lstrlenW(Offset));
		TextOut(_dc, (int)(m_vResolution.x - (m_vResolution.x / 4) + 10), 130, Duration, lstrlenW(Duration));
}
void CAnimEditor::PrintMousePos(HDC _dc)
{
	Vec2 MousePressPos = MOUSE_POS;
	Vec2 RealPressPos = CCamera::GetInst()->GetRealPos(MousePressPos) / m_fmagni;

	TCHAR MousePos[128] = {};
	wsprintf(MousePos, TEXT("X : %d / Y : %d"), (UINT)(RealPressPos.x), (UINT)(RealPressPos.y));

	SetTextAlign(_dc, TA_LEFT);
	TextOut(_dc, 20, (int)m_vResolution.y - 20, MousePos, lstrlenW(MousePos));
}

void CAnimEditor::PlayLeft()
{
	if(m_bAnimator1)
	m_pEditPlayer->GetAnimator()->GetAnimation()->SetPlayLeft();
	if(m_bAnimator2)
	m_pEditPlayer->GetAnimator2()->GetAnimation()->SetPlayLeft();
}

void CAnimEditor::PlayRight()
{
	if (m_bAnimator1)
		m_pEditPlayer->GetAnimator()->GetAnimation()->SetPlayRight();
	if (m_bAnimator2)
		m_pEditPlayer->GetAnimator2()->GetAnimation()->SetPlayRight();
}

