#include "pch.h"
#include "CTrace.h"

#include "CTimeMgr.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CPlayer.h"

#include "CMonster.h"
#include "CCamera.h"

#include "CEngine.h"

CTrace::CTrace()
{
}

CTrace::~CTrace()
{
}

void CTrace::final_tick()
{
	// 상태 소유 오브젝트가 몬스터 종류가 아닌 경우
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

	float fSpeed = pMon->GetMstInfo().m_fSpeed;

	vMonPos += vMonToPlayer * DT * fSpeed;

	float fAttackRange = pMon->GetMstInfo().m_fAttackRange;
	float fDetectRange = pMon->GetMstInfo().m_fDetectRange;
		// Player 가 탐지범위 이내에 들어오면 추적상태로 전환
	
	Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
	if (pMon->GetMstInfo().m_iHP < 1)// hp가 0이면 죽음

		ChangeState(L"Dead");
	else if (vDir.Length() < fAttackRange) //공격 사거리보다 가까우면 공격상태로
		ChangeState(L"Attack");
	else if (vDir.Length() > fDetectRange) // 공격보다 멀면
		ChangeState(L"Idle");
	
	

	pMon->SetPos(vMonPos);
}

void CTrace::Enter()
{
}

void CTrace::Exit()
{
}
