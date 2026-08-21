#include "pch.h"
#include "CBackGround_Back.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CBackGround_Back::CBackGround_Back()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_vecResolution{}
	, m_fmagni(6.f)
{
	m_vecResolution = CEngine::GetInst()->GetResolution();
	m_pBackBuffer = m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_BackBackBuffer", (UINT)(m_vecResolution.x), (UINT)(m_vecResolution.y));
	SetPos(Vec2(0.f,0.f));
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"BackGround_Back",  L"texture\\BackGround_Back.bmp");
}

CBackGround_Back::~CBackGround_Back()
{
}

void CBackGround_Back::tick()
{
	CObj::tick();
}

void CBackGround_Back::render(HDC _dc)
{
	Vec2 vPos = GetPos();
	vPos = CCamera::GetInst()->GetRealPos(vPos);

	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_vecResolution.x), (int)(m_vecResolution.y)
		, m_pAtlas->GetDC()
		, (int)(vPos.x / 3.3)
		, (int)(vPos.y / 3.3) + 40
		, (int)(m_vecResolution.x / m_fmagni), (int)(m_vecResolution.y / m_fmagni)
		, SRCCOPY);

	TransparentBlt(_dc
		, 0
		, 0
		, int(m_vecResolution.x)
		, int(m_vecResolution.y)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_vecResolution.x), (int)(m_vecResolution.y)
		, RGB(255, 0, 255));

	CObj::render(_dc);
}