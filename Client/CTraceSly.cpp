#include "pch.h"
#include "CTraceSly.h"

#include "CTimeMgr.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CPlayer.h"

#include "CMonster.h"

CTraceSly::CTraceSly()
{
}

CTraceSly::~CTraceSly()
{
}

void CTraceSly::final_tick()
{
    // 소유 오브젝트가 Monster가 맞는지 확인(캐스팅)
    CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
    assert(pMon);

    // Player 를 알아낸다.
    CLevel*  pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
    CPlayer* pPlayer   = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

    assert(pPlayer);

    Vec2       vMonPos    = pMon->GetPos();
    const Vec2 vPlayerPos = pPlayer->GetPos();

    Vec2 vMonToPlayer = vPlayerPos - vMonPos;
    vMonToPlayer.Normalize();

    SetMontoPlayer(vMonToPlayer);

    const float fSpeed = pMon->GetMstInfo().m_fSpeed;

    vMonPos += vMonToPlayer * DT * fSpeed / 2;

    const float fAttackRange = pMon->GetMstInfo().m_fAttackRange;

    // Player 가 공격범위 이내로 들어오면 공격상태로 전환
    const Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
    if (pMon->GetMstInfo().m_iHP < 1)
    {
        ChangeState(L"Dead");
    }
    else if (vDir.Length() < fAttackRange)
    {
        if (vDir.Length() < fAttackRange * 0.2f)
            ChangeState(L"AttackCQC");
    }
    else if (vDir.Length() > fAttackRange)
    {
        ChangeState(L"Trace");
    }

    pMon->SetPos(vMonPos);
}

void CTraceSly::Enter()
{
}

void CTraceSly::Exit()
{
}
