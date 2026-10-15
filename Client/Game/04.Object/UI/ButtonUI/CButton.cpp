#include "pch.h"
#include "Game\04.Object\UI\ButtonUI\CButton.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

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
		// 숫자 애니메이션의 m_iCurFrm번 프레임(숫자 그림)을 읽기만 한다. 범위 밖이면 가장 가까운 프레임
		// (예전에는 render가 매번 재생하고 프레임을 바꿔 그리기가 상태를 바꿨다)
		CAnimator*        pAnimator = m_bTime ? GetAnimator2() : GetAnimator();
		const CAnimation* pNumber   = pAnimator->FindAnimation(m_bTime ? L"TIME_NUMBER.anim" : L"Number.anim");
		const tAnimFrm    tFrm      = nullptr == pNumber ? tAnimFrm{} : pNumber->GetFrm(m_iCurFrm);
		m_vLeftTop = tFrm.vLeftTop;
		m_vSize    = tFrm.vSize;
		m_fOffset  = tFrm.vOffset;
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

