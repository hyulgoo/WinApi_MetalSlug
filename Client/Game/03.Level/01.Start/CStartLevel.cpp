#include "pch.h"
#include "Game\03.Level\01.Start\CStartLevel.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\04.Object\CObj.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\04.Object\Title\CSelectAnim.h"
#include "Game\04.Object\BackGround\CStartBG.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CStartLevel::CStartLevel()
	: m_bStart(false)
	, m_pSelectAnim(nullptr)
	, m_fTime(0.f)
	, m_bEnd(false)
{
}

CStartLevel::~CStartLevel()
{
}

void CStartLevel::tick()
{
	if (IsTap(KEY::C))
	{
		m_pSelectAnim->SetStart();
		CCamera::GetInst()->CameraShake(m_tData.fShakeRange, m_tData.fShakeSpeed, m_tData.fShakeTime);
		m_bStart = true;
	}

	if (IsTap(KEY::LEFT) || IsTap(KEY::RIGHT))
		m_pSelectAnim->SetNumber();

	// 애니메이션 에디터 (도구라서 Release에서도 들어갈 수 있다)
	if (!m_bStart && IsTap(KEY::F1))
		ChangeLevel(LEVEL_TYPE::ANIMEDITOR);

	if (m_bStart)
	{
		m_fTime += DT;

		if (m_fTime > m_tData.fFadeOutAt)
		{
			if (!m_bEnd)
			{
				CCamera::GetInst()->FadeOut(m_tData.fFadeOutTime);
				m_bEnd = true;
			}
		}

		if (m_fTime > m_tData.fStageAt)
			ChangeLevel(LEVEL_TYPE::STAGE);
	}

	CLevel::tick();
}

void CStartLevel::render(const HDC _dc)
{
	CLevel::render(_dc);

	constexpr LPCTSTR szHelp = L"C : START    F1 : ANIMATION EDITOR";
	const int      iOldMode  = SetBkMode(_dc, TRANSPARENT);
	const COLORREF tOldColor = SetTextColor(_dc, m_tData.tHelpColor);
	TextOut(_dc, static_cast<int>(m_tData.vHelpPos.x), static_cast<int>(m_tData.vHelpPos.y), szHelp, lstrlen(szHelp));
	SetTextColor(_dc, tOldColor);
	SetBkMode(_dc, iOldMode);
}

void CStartLevel::Enter()
{
	// 들어올 때마다 오브젝트를 새로 만들고 진행 상태를 처음으로 돌린다
	m_bStart = false;
	m_fTime  = 0.f;
	m_bEnd   = false;

	const CDataSection& tTitle = CDataMgr::GetInst()->GetSection(L"title", L"title");
	m_tData.fShakeRange        = tTitle.GetFloat(L"shake_range");
	m_tData.fShakeSpeed        = tTitle.GetFloat(L"shake_speed");
	m_tData.fShakeTime         = tTitle.GetFloat(L"shake_time");
	m_tData.fFadeOutAt         = tTitle.GetFloat(L"fade_out_at");
	m_tData.fFadeOutTime       = tTitle.GetFloat(L"fade_out_time");
	m_tData.fStageAt           = tTitle.GetFloat(L"stage_at");
	m_tData.vHelpPos           = tTitle.GetVec2(L"help_pos");
	m_tData.tHelpColor         = tTitle.GetColor(L"help_color");
	const Vec2 vSelectPos      = tTitle.GetVec2(L"select_pos");

	CStartBG* pStartBG = new CStartBG;
	pStartBG->SetPos(Vec2(0.f, 0.f));
	AddObject(pStartBG, LAYER::BACKGROUND);

	m_pSelectAnim = new CSelectAnim;
	m_pSelectAnim->SetPos(vSelectPos);
	AddObject(m_pSelectAnim, LAYER::PLAYER);
}

void CStartLevel::Exit()
{
	DeleteAllObjects();
	m_pSelectAnim = nullptr;
}
