#include "pch.h"
#include "CStage01Level.h"
#include "CGround.h"
#include "CPlayer.h"
#include "CObj.h"
#include "CCollider.h"
#include "CKeyMgr.h"


CStage01Level::CStage01Level()
{
}

CStage01Level::~CStage01Level()
{
}


void CStage01Level::init()
{
	//CGround* pGround = new CGround;
	//pGround->SetPos(Vec2(1600.f, 800.f));
	//pGround->GetCollider()->SetScale(Vec2(3200.f, 100.f));
	//AddObject(pGround, LAYER::TILE);


	// Player »ý¼º	
	CObj* m_pPlayer = new CPlayer;
	m_pPlayer->SetPos(Vec2(800.f, 400.f));
	m_pPlayer->SetScale(Vec2(100.f, 100.f));
	AddObject(m_pPlayer, LAYER::PLAYER);
}

void CStage01Level::tick()
{
	CLevel::tick();
	if (IsTap(KEY::ENTER))
	{
		ChangeLevel(LEVEL_TYPE::START);
	}
}

void CStage01Level::Enter()
{
	init();
}

void CStage01Level::Exit()
{
	DeleteObject();
}
