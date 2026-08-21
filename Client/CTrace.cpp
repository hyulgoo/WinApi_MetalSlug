#include "pch.h"
#include "CTrace.h"

#include "CTimeMgr.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CPlayer.h"

#include "CMonster.h"

CTrace::CTrace()
{
}

CTrace::~CTrace()
{
}

void CTrace::final_tick()
{
    // 이 컴포넌트의 소유 오브젝트가 몬스터가 아닌 경우 방어
    CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
    assert(pMon);

    // Player 를 찾아낸다.
    CLevel*  pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
    CPlayer* pPlayer   = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

    assert(pPlayer);

    Vec2       vMonPos    = pMon->GetPos();
    const Vec2 vPlayerPos = pPlayer->GetPos();

    Vec2 vMonToPlayer = vPlayerPos - vMonPos;
    vMonToPlayer.Normalize();

    SetMontoPlayer(vMonToPlayer);

    const float fSpeed = pMon->GetMstInfo().m_fSpeed;

    vMonPos += vMonToPlayer * DT * fSpeed;

    const float fAttackRange = pMon->GetMstInfo().m_fAttackRange;
    const float fDetectRange = pMon->GetMstInfo().m_fDetectRange;
    // Player 와의 거리에 따라 상태를 전환한다 (사망/공격/대기)

    const Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
    if (pMon->GetMstInfo().m_iHP < 1) // hp가 0이면 사망
        ChangeState(L"Dead");
    else if (vDir.Length() < fAttackRange) //공격 사거리 안쪽이면 공격상태로
        ChangeState(L"Attack");
    else if (vDir.Length() > fDetectRange) // 탐지범위보다 멀면
        ChangeState(L"Idle");

    pMon->SetPos(vMonPos);
}

void CTrace::Enter()
{
}

void CTrace::Exit()
{
}
