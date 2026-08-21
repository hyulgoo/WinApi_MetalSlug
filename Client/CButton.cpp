#include "pch.h"
#include "CButton.h"
#include "CTexture.h"
#include "CAnimation.h"
#include "CAnimator.h"
#include "CResMgr.h"
#include "CPlayer.h"

CButton::CButton()
	: m_iCurFrm(2)
	, m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_vLeftTop(Vec2(80.f,27.f))
	, m_vSize(Vec2(12.f, 12.f))
	, m_bNone(false)
	, m_bLife(false)
	, m_bTime(false)
{
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Number", L"texture//NUMBER.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"NumberBackBuffer", 300, 300);
	CreateAnimator();
	CreateAnimator2();
	GetAnimator()->LoadAnimation(L"Number.anim");
	GetAnimator2()->LoadAnimation(L"TIME_NUMBER.anim");
}

CButton::~CButton()
{
}

void CButton::render(const HDC _dc)
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
			, static_cast<int>(m_vSize.x * 4), static_cast<int>(m_vSize.y * 4)
			, m_pAtlas->GetDC()
			, static_cast<int>(m_vLeftTop.x)
			, static_cast<int>(m_vLeftTop.y)
			, static_cast<int>(m_vSize.x)
			, static_cast<int>(m_vSize.y), SRCCOPY);
		TransparentBlt
		(_dc
			, static_cast<int>(GetFinalPos().x + m_fOffset.x * 4), static_cast<int>(GetFinalPos().y + m_fOffset.y * 4)
			, static_cast<int>(m_vSize.x * 4)
			, static_cast<int>(m_vSize.y * 4)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, static_cast<int>(m_vSize.x * 4)
			, static_cast<int>(m_vSize.y * 4)
			, RGB(255, 0, 255));
	}

	if (m_bLife)
	{
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, 26 * 4, 10 * 4
			, m_pAtlas->GetDC()
			, 0
			, 27
			, 26
			, 10, SRCCOPY);
		TransparentBlt
		(_dc
			, static_cast<int>(GetFinalPos().x - 140), static_cast<int>(GetFinalPos().y + m_fOffset.y * 4)
			, 26 * 4
			, 10 * 4
			, m_pBackBuffer->GetDC()
			, 0, 0
			, 26 * 4
			, 10 * 4
			, RGB(255, 0, 255));
	}
	// 0 27 26 37
	render_ChildUI(_dc);

}

