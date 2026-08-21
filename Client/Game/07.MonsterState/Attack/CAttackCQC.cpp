#include "pch.h"
#include "Game\07.MonsterState\Attack\CAttackCQC.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

#include "Game\04.Object\Character\Monster\CMonster.h"

CAttackCQC::CAttackCQC()
    : m_fAttackDelay(0)
    , m_bDetail(false)
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
    CLevel*  pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
    CPlayer* pPlayer   = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

    assert(pPlayer);

    const Vec2 vMonToPlayer = pPlayer->GetPos() - pMon->GetPos();

    SetMontoPlayer(vMonToPlayer);

    // Player 와 Monster 의 거리를 계산
    const float fAttackRange = pMon->GetMstInfo().m_fAttackRange;
    const float fDetectRange = pMon->GetMstInfo().m_fDetectRange;

    // Player 와의 거리에 따라 상태를 전환한다.
    const Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();
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
    // 공격범위를 벗어나면 Trace 상태로 전환한다.
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
