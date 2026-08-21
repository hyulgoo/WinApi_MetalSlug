#include "pch.h"
#include "CEditAnim.h"
#include "CAnimator.h"
#include "CAnimation.h"

#include "CEngine.h"
#include "CResMgr.h"
#include "CTexture.h"


CEditAnim::CEditAnim()
	: m_AnimPath()
	, m_vStart()
	, m_vEnd()
	, m_pAtlas(nullptr)
	, m_fmagni(3.f)
{
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
CEditAnim::~CEditAnim()
{
}

void CEditAnim::tick()
{
	CObj::tick();
}

void CEditAnim::render(HDC _dc)
{
	// 바탕으로 깔아줄 bmp를 설정함.
	Vec2 vPos = CCamera::GetInst()->GetRenderPos(GetPos());
	StretchBlt(CEngine::GetInst()->GetMemTexDC()
		, (int)vPos.x, (int)vPos.y
		, (int)(m_pAtlas->Width() * m_fmagni), (int)(m_pAtlas->Height() * m_fmagni)
		, m_pAtlas->GetDC()
		, 0, 0
		, m_pAtlas->Width(), m_pAtlas->Height(),
		SRCCOPY);

	CObj::render(_dc);
}

