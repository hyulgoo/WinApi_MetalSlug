#include "pch.h"
#include "Game\04.Object\UI\EditAnim\CEditAnim.h"

#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

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

void CEditAnim::render(const HDC _dc)
{
	// 에디터에서 사용할 bmp를 그린다.
	const Vec2 vPos = CCamera::GetInst()->GetRenderPos(GetPos());
	StretchBlt(CEngine::GetInst()->GetMemTexDC()
		, static_cast<int>(vPos.x), static_cast<int>(vPos.y)
		, static_cast<int>(m_pAtlas->Width() * m_fmagni), static_cast<int>(m_pAtlas->Height() * m_fmagni)
		, m_pAtlas->GetDC()
		, 0, 0
		, m_pAtlas->Width(), m_pAtlas->Height(),
		SRCCOPY);

	CObj::render(_dc);
}

