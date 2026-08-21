#include "pch.h"
#include "CCharcter.h"

#include "CResMgr.h"
#include "CTexture.h"
#include "CEngine.h"

CCharcter::CCharcter()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_fmagni(4.f)
{
	//m_vResolution = CEngine::GetInst()->GetResolution();
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Select", L"texture\\Select.bmp");
	//m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"CRTBackBuffer", (int)m_vResolution.x, (int)m_vResolution.y);
	//SetPos(Vec2(0.f, 0.f));
}

CCharcter::~CCharcter()
{
}

void CCharcter::tick()
{

	CObj::tick();
}

void CCharcter::render(HDC _dc)
{
	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, (int)(m_vResolution.x), (int)(m_vResolution.y)
		, m_pAtlas->GetDC()
		, 0
		, 0
		, (int)(m_pAtlas->Width()), (int)(m_pAtlas->Height())
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
