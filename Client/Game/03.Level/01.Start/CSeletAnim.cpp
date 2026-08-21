#include "pch.h"
#include "Game\03.Level\01.Start\CSeletAnim.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\06.Resource\Texture\CTexture.h"

CSeletAnim::CSeletAnim()
	: m_pAtlas(nullptr)
	, m_bStart(false)
	, m_iCurFrm(0)
	, m_fTime(0.f)
	, m_fmagni(3.8f)
	, m_bPrev(false)
{
	CreateAnimator();
	CreateAnimator2();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Select", L"texture\\Select_Screen.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"SelectBuffer", static_cast<int>(m_pAtlas->Width() * m_fmagni), static_cast<int>(m_pAtlas->Height() * m_fmagni));
	GetAnimator()->LoadAnimation(L"ShutDown.anim");
	GetAnimator2()->LoadAnimation(L"SelectEri.anim");
	GetAnimator2()->LoadAnimation(L"ShutDown_Effect.anim");
	GetAnimator2()->LoadAnimation(L"NONE.anim");
}

CSeletAnim::~CSeletAnim()
{
}

void CSeletAnim::render(const HDC _dc)
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
			, static_cast<int>(m_vSize.x * m_fmagni), static_cast<int>(m_vSize.y * m_fmagni)
			, m_pAtlas->GetDC()
			, static_cast<int>(m_vLeftTop.x)
			, static_cast<int>(m_vLeftTop.y)
			, static_cast<int>(m_vSize.x)
			, static_cast<int>(m_vSize.y), SRCCOPY);
		TransparentBlt
		(_dc
			, static_cast<int>(GetPos().x + m_fOffset.x * m_fmagni), static_cast<int>(GetPos().y + m_fOffset.y * m_fmagni)
			, static_cast<int>(m_vSize.x * m_fmagni)
			, static_cast<int>(m_vSize.y * m_fmagni)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, static_cast<int>(m_vSize.x * m_fmagni)
			, static_cast<int>(m_vSize.y * m_fmagni)
			, RGB(255, 0, 255));
	}

	CObj::render(_dc);
	m_bPrev = m_bStart;
}
