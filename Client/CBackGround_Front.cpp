#include "pch.h"
#include "CBackGround_Front.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CBackGround_Front::CBackGround_Front()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_vecResolution{}
	, m_fmagni(4.f)
{
	m_vecResolution = CEngine::GetInst()->GetResolution();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"BackGround_Front", L"texture\\BackGround_Front01.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_FrontBackBuffer", (UINT)(m_pAtlas->Width() * m_fmagni), (UINT)(m_pAtlas->Height() * m_fmagni));
	SetPos(Vec2(3658.f,508));
}

CBackGround_Front::CBackGround_Front(const CBackGround_Front& _Other)
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_vecResolution(_Other.m_vecResolution)
	, m_fmagni(_Other.m_fmagni)
{
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"BackGround_Front2", L"texture\\BackGround_Front02.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_FrontBackBuffer2", (UINT)(m_pAtlas->Width() * m_fmagni), (UINT)(m_pAtlas->Height() * m_fmagni));
	SetPos(Vec2(5815.f, 322));
}

CBackGround_Front::~CBackGround_Front()
{
}

void CBackGround_Front::tick()
{
	CObj::tick();
}

void CBackGround_Front::render(HDC _dc)
{
	Vec2 vPos = GetPos();
	Vec2 vDifPos = CCamera::GetInst()->GetRenderPos(Vec2(0.f,0.f));

	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_pAtlas->Width() * m_fmagni), (int)(m_pAtlas->Height() * m_fmagni)
		, m_pAtlas->GetDC()
		, 0
		, 0
		, (int)(m_pAtlas->Width()), (int)(m_pAtlas->Height())
		, SRCCOPY);

	TransparentBlt(_dc
		, (int)(vPos.x + vDifPos.x * m_fmagni)
		, (int)(vPos.y + vDifPos.y * m_fmagni)
		, int(m_pAtlas->Width() * m_fmagni)
		, int(m_pAtlas->Height() * m_fmagni)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_pAtlas->Width() * m_fmagni), (int)(m_pAtlas->Height() * m_fmagni)
		, RGB(255, 0, 255));

	CObj::render(_dc);
}