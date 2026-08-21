#include "pch.h"
#include "CAttackCQC.h"

#include "CTimeMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CPlayer.h"

#include "CMonster.h"

CAttackCQC::CAttackCQC()
	: m_bDetail(false)
{
}

CAttackCQC::~CAttackCQC()
{
}
void CAttackCQC::final_tick()
{
	CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
	assert(pMon);

	// Player 를 알아낸다.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

	assert(pPlayer);

	Vec2 vMonToPlayer = pPlayer->GetPos() - pMon->GetPos();

	SetMontoPlayer(vMonToPlayer);

	// Player 와 Monster 의 거리값을 계산
	float fAttackRange = pMon->GetMstInfo().m_fAttackRange;
	float fDetectRange = pMon->GetMstInfo().m_fDetectRange;

	// Player 가 공격범위 이내에 들어오면 공격상태로 전환함.
	Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
	if (pMon->GetMstInfo().m_iHP < 1)
	{
			ChangeState(L"Dead");
	}

	else if (pMon->IsAttack())
	{
		m_fAttackDelay += DT;
		if (m_fAttackDelay > 1.6f)
		{
			pMon->SetAttack(false);
			m_fAttackDelay = 0.f;
			ChangeState(L"Trace");

		}
	}
	else if (vDir.Length() < fAttackRange * 0.2f)
	{
			pMon->SetAttack(true);
	}
	else if (vDir.Length() > fAttackRange * 0.5f)
	{
		ChangeState(L"Attack");
	}
	else if (fAttackRange * 0.2f < vDir.Length() && vDir.Length() < fAttackRange * 0.5f)
	{
		ChangeState(L"TraceSly");
	}
		// 공격상태 중이면 공격 딜레이만큼 시간을 세고 지났다면 공격상태를 해제함
	else if (vDir.Length() > fAttackRange)
	{
		ChangeState(L"Trace");
	}
	else if (vDir.Length() > fDetectRange)
	{
		ChangeState(L"Idle");
	}
	
}

void CAttackCQC::Enter()
{
}

void CAttackCQC::Exit()
{
}

