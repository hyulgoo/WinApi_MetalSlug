#include "pch.h"
#include "CEditPlayer.h"
#include "CTexture.h"
#include "CResMgr.h"
#include "CAnimator.h"
#include "CEngine.h"

CEditPlayer::CEditPlayer()
	: m_pAtlas(nullptr)
	, m_pAnimator(nullptr)
{
	const Vec2 vResolution = CEngine::GetInst()->GetResolution();
	CreateAnimator();
	CreateAnimator2();
	SetPos(Vec2(vResolution.x - vResolution.x / 8, vResolution.y / 9 * 5));
	
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Player", L"texture\\Eri.bmp");
	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Soldier", L"texture\\Soldier.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"NPC", L"texture\\Helpful_NPC.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Boss01", L"texture\\Boss01.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Number", L"texture\\NUMBER.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"SoldierSFX", L"texture\\SFX.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Text", L"texture\\TEXTUI.bmp");
	//m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Start", L"texture\\Select_Screen.bmp");
}

CEditPlayer::~CEditPlayer()
{
}

void CEditPlayer::tick()
{
	CObj::tick();
}

void CEditPlayer::render(const HDC _dc)
{
	CObj::render(_dc);

	const Vec2 vPos = GetPos();
	const Vec2 vScale = Vec2(50.f, 44.f);
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
