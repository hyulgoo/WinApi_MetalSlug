#include "pch.h"
#include "CSeletAnim.h"
#include "CResMgr.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CTexture.h"
#include "CTimeMgr.h"

CSeletAnim::CSeletAnim()
	: m_pAtlas(nullptr)
	, m_bStart(false)
	, m_fTime(0.f)
	, m_iCurFrm(0)
	, m_fmagni(3.8f)
	, m_bPrev(false)
{
	CreateAnimator();
	CreateAnimator2();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Select", L"texture\\Select_Screen.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"SelectBuffer", (int)(m_pAtlas->Width() * m_fmagni), (int)(m_pAtlas->Height() * m_fmagni));
	GetAnimator()->LoadAnimaton(L"ShutDown.anim");
	GetAnimator2()->LoadAnimaton(L"SelectEri.anim");
	GetAnimator2()->LoadAnimaton(L"ShutDown_Effect.anim");
	GetAnimator2()->LoadAnimaton(L"NONE.anim");
}

CSeletAnim::~CSeletAnim()
{
}

void CSeletAnim::render(HDC _dc)
{	
	if (m_bStart)
	{
		if (m_bPrev != m_bStart)
		{
			GetAnimator()->Play(L"ShutDown.anim", false);
			GetAnimator()->GetAnimation()->SetMagni(4);
			GetAnimator2()->Play(L"ShutDown_Effect.anim", false);
			GetAnimator2()->GetAnimation()->SetMagni(4); 
		}
	}	
	else
	{
		GetAnimator2()->Play(L"SelectEri.anim", false);
		GetAnimator2()->GetAnimation()->SetMagni(m_fmagni);
		GetAnimator2()->GetAnimation()->SetFrmNum(m_iCurFrm);
		m_vLeftTop = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vLeftTop;
		m_vSize = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vSize;
		m_fOffset = GetAnimator2()->GetAnimation()->GetFrmInfo()[m_iCurFrm].vOffset;
		
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, int(m_vSize.x * m_fmagni), int(m_vSize.y * m_fmagni)
			, m_pAtlas->GetDC()
			, int(m_vLeftTop.x)
			, int(m_vLeftTop.y)
			, int(m_vSize.x)
			, int(m_vSize.y), SRCCOPY);
		TransparentBlt
		(_dc
			, (int)(GetPos().x + m_fOffset.x * m_fmagni), (int)(GetPos().y + m_fOffset.y * m_fmagni)
			, int(m_vSize.x * m_fmagni)
			, int(m_vSize.y * m_fmagni)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, int(m_vSize.x * m_fmagni)
			, int(m_vSize.y * m_fmagni)
			, RGB(255, 0, 255));
	}

	CObj::render(_dc);
	m_bPrev = m_bStart;
}
