#include "pch.h"
#include "Game\04.Object\Character\CCharacter.h"

#include <wingdi.h>

#include "Game\06.Resource\Texture\CTexture.h"

CCharacter::CCharacter()
	: m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_fmagni(4.f)
{
	//m_vResolution = CEngine::GetInst()->GetResolution();
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Select", L"texture\\Select.bmp");
	//m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"CRTBackBuffer", (int)m_vResolution.x, (int)m_vResolution.y);
	//SetPos(Vec2(0.f, 0.f));
}

CCharacter::~CCharacter()
{
}

void CCharacter::tick()
{

	CObj::tick();
}

void CCharacter::render(const HDC _dc)
{
	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
		, m_pAtlas->GetDC()
		, 0
		, 0
		, static_cast<int>(m_pAtlas->Width()), static_cast<int>(m_pAtlas->Height())
		, SRCCOPY);
	TransparentBlt(_dc
		, 0
		, 0
		, static_cast<int>(m_vResolution.x)
		, static_cast<int>(m_vResolution.y)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y)
		, RGB(255, 0, 255));

	CObj::render(_dc);
}
