#include "pch.h"
#include "CIdle.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CPlayer.h"

#include "CMonster.h"
#include "CTimeMgr.h"

CIdle::CIdle()
{
}

CIdle::~CIdle()
{
}

void CIdle::final_tick()
{
	CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
	assert(pMon);

	// Player 를 알아낸다.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

	assert(pPlayer);

	Vec2 vMonPos = pMon->GetPos();
	Vec2 vPlayerPos = pPlayer->GetPos();

	Vec2 vMonToPlayer = vPlayerPos - vMonPos;
	vMonToPlayer.Normalize();

	SetMontoPlayer(vMonToPlayer);

	// Player 와 Monster 의 거리값을 계산
	float fDetectRange = pMon->GetMstInfo().m_fDetectRange;


	// Player 가 탐지범위 이내에 들어오면 추적상태로 전환
	Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
	if (pMon->GetMstInfo().m_iHP < 1)
		ChangeState(L"Dead");
	else if (vDir.Length() < fDetectRange)
	{
		ChangeState(L"Trace");
	}
}