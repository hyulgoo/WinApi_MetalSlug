#include "pch.h"
#include "CButton.h"
#include "CTexture.h"
#include "CAnimation.h"
#include "CAnimator.h"
#include "CResMgr.h"
#include "CPlayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"

CButton::CButton()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_vLeftTop(Vec2(80.f,27.f))
	, m_vSize(Vec2(12.f, 12.f))
	, m_iCurFrm(2)
	, m_bNone(false)
	, m_bLife(false)
	, m_bTime(false)
{
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Number", L"texture//NUMBER.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"NumberBackBuffer", 300, 300);
	CreateAnimator();
	CreateAnimator2();
	GetAnimator()->LoadAnimaton(L"Number.anim");
	GetAnimator2()->LoadAnimaton(L"TIME_NUMBER.anim");
}

CButton::~CButton()
{
}

void CButton::render(HDC _dc)
{
	if (!m_bNone)
	{
		if (!m_bTime)
		{
			GetAnimator()->Play(L"Number.anim", false);
			GetAnimator()->GetAnimation()->SetFrmNum(m_iCurFrm);
			m_vLeftTop = GetAnimator()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vLeftTop;
			m_vSize = GetAnimator()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vSize;
			m_fOffset = GetAnimator()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vOffset;
		}
		else if(m_bTime)
		{
			GetAnimator2()->Play(L"TIME_NUMBER.anim", false);
			GetAnimator2()->GetAnimation()->SetFrmNum(m_iCurFrm);
			m_vLeftTop = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vLeftTop;
			m_vSize = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vSize;
			m_fOffset = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vOffset;
		}
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, int(m_vSize.x * 4), int(m_vSize.y * 4)
			, m_pAtlas->GetDC()
			, int(m_vLeftTop.x)
			, int(m_vLeftTop.y)
			, int(m_vSize.x)
			, int(m_vSize.y), SRCCOPY);
		TransparentBlt
		(_dc
			, (int)(GetFinalPos().x + m_fOffset.x * 4), (int)(GetFinalPos().y + m_fOffset.y * 4)
			, int(m_vSize.x * 4)
			, int(m_vSize.y * 4)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, int(m_vSize.x * 4)
			, int(m_vSize.y * 4)
			, RGB(255, 0, 255));
	}

	if (m_bLife)
	{
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, int(26 * 4), int(10 * 4)
			, m_pAtlas->GetDC()
			, 0
			, 27
			, 26
			, 10, SRCCOPY);
		TransparentBlt
		(_dc
			, (int)(GetFinalPos().x - 140), (int)(GetFinalPos().y + m_fOffset.y * 4)
			, int(26 * 4)
			, int(10 * 4)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, int(26 * 4)
			, int(10 * 4)
			, RGB(255, 0, 255));
	}
	// 0 27 26 37
	render_ChildUI(_dc);

}

