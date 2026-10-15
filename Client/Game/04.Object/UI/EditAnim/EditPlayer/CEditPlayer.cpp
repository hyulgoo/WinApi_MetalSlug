#include "pch.h"
#include "Game\04.Object\UI\EditAnim\EditPlayer\CEditPlayer.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CEditPlayer::CEditPlayer()
{
	m_vGuide = CDataMgr::GetInst()->GetSection(L"editor", L"view").GetVec2(L"guide_size");
	CreateAnimator();
	CreateAnimator2();
}

CEditPlayer::~CEditPlayer()
{
}

void CEditPlayer::tick()
{
	if (!m_bPause)
		CObj::tick();
}

void CEditPlayer::render(const HDC _dc)
{
	CAnimator* pDoc = GetAnimator();
	CAnimator* pRef = GetAnimator2();
	if (m_bDocBelow)
	{
		pDoc->render(_dc);
		pRef->render(_dc);
	}
	else
	{
		pRef->render(_dc);
		pDoc->render(_dc);
	}

	// 기준선: 세로는 오브젝트 위치, 가로는 그 아래 (오프셋을 맞추는 기준)
	const Vec2 vPos = GetPos();
	const Vec2 vScale = m_vGuide;
	MoveToEx(_dc,
		static_cast<int>(vPos.x - vScale.x),
		static_cast<int>(vPos.y + vScale.y),
		nullptr);
	LineTo(_dc,
		static_cast<int>(vPos.x + vScale.x),
		static_cast<int>(vPos.y + vScale.y));
	MoveToEx(_dc,
		static_cast<int>(vPos.x),
		static_cast<int>(vPos.y - vScale.y),
		nullptr);
	LineTo(_dc,
		static_cast<int>(vPos.x),
		static_cast<int>(vPos.y + vScale.y));
}
