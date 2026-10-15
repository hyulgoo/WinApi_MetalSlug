#include "pch.h"
#include "Game\04.Object\Title\CSelectAnim.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\06.Resource\Texture\CTexture.h"

namespace
{
	constexpr float SELECT_MAGNI = 3.8f;   // 캐릭터 선택 화면 배율 (화면을 채우게)
}

CSelectAnim::CSelectAnim()
	: m_bStart(false)
	, m_iCurFrm(0)
	, m_fmagni(SELECT_MAGNI)
	, m_bPrev(false)
{
	CreateAnimator();
	CreateAnimator2();
	GetAnimator()->LoadAnimation(L"ShutDown.anim");
	GetAnimator2()->LoadAnimation(L"SelectEri.anim");
	GetAnimator2()->LoadAnimation(L"ShutDown_Effect.anim");
	GetAnimator2()->LoadAnimation(L"NONE.anim");
}

CSelectAnim::~CSelectAnim()
{
}

void CSelectAnim::tick()
{
	if (m_bStart)
	{
		// 캐릭터를 고른 순간 셔터가 내려오는 애니메이션을 한 번 튼다
		if (!m_bPrev)
		{
			GetAnimator()->Play(L"ShutDown.anim", false);
			GetAnimator()->GetAnimation()->SetMagni(4);
			GetAnimator2()->Play(L"ShutDown_Effect.anim", false);
			GetAnimator2()->GetAnimation()->SetMagni(4);
		}
	}
	else if (nullptr == GetAnimator2()->GetAnimation() || L"SelectEri.anim" != GetAnimator2()->GetAnimation()->GetName())
	{
		GetAnimator2()->Play(L"SelectEri.anim", false);
		GetAnimator2()->GetAnimation()->SetMagni(m_fmagni);
	}
	m_bPrev = m_bStart;

	CObj::tick();

	// 고르기 전에는 SelectEri의 m_iCurFrm번 프레임(선택 표시)을 그대로 보여 준다
	if (!m_bStart)
		GetAnimator2()->GetAnimation()->SetFrmNum(m_iCurFrm);
}

void CSelectAnim::render(const HDC _dc)
{
	// 예전에는 render가 애니메이션을 재생하고 프레임을 바꾼 뒤, 같은 프레임을 직접 한 번 더 그렸다
	CObj::render(_dc);
}
