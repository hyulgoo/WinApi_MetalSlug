#include "pch.h"
#include "CBackGround.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CBackGround::CBackGround()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_fmagni(4.f)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"BackGroundasd", L"texture\\BackGround.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BackGround_BackBuffer",(UINT)(m_vResolution.x + 150),(UINT)(m_vResolution.y));
	SetPos(Vec2(0.f,0.f));
}

CBackGround::~CBackGround()
{
}

void CBackGround::tick()
{
	CObj::tick();
}

void CBackGround::render(HDC _dc)
{	
	Vec2 vPos = GetPos();
	vPos = CCamera::GetInst()->GetRealPos(vPos);

	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_vResolution.x), (int)(m_vResolution.y)
		, m_pAtlas->GetDC()
		, (int)(vPos.x)
		, (int)(vPos.y)
		, (int)(m_vResolution.x / m_fmagni), (int)(m_vResolution.y / m_fmagni)
		, SRCCOPY);

	TransparentBlt(_dc
		, 0
		, 0
		, int(m_vResolution.x)
		, int(m_vResolution.y)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_vResolution.x), (int)(m_vResolution.y)
		, RGB(255, 0, 255));

	CObj::render(_dc);
}
