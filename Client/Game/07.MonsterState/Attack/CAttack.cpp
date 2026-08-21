#include "pch.h"
#include "Game\07.MonsterState\Attack\CAttack.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

#include "Game\04.Object\Character\Monster\CMonster.h"

CAttack::CAttack()
    : m_fAttackDelay()
    , m_bNowAttack(false)
{
}

CAttack::~CAttack()
{
}

void CAttack::final_tick()
{
    CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
    assert(pMon);

    // Player 를 알아낸다.
    CLevel*  pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
    CPlayer* pPlayer   = dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0]);

    assert(pPlayer);

    Vec2 vMonPos    = pMon->GetPos();
    Vec2 vPlayerPos = pPlayer->GetPos();

    const Vec2 vMonToPlayer = pPlayer->GetPos() - pMon->GetPos();

    SetMontoPlayer(vMonToPlayer);

    // Player 와 Monster 간 거리를 계산
    const float fAttackRange = pMon->GetMstInfo().m_fAttackRange;
    const float fAttackDelay = pMon->GetMstInfo().m_fAttackDelay;

    // Player 가 공격범위 이내로 들어오면 공격상태로 전환한다.
    const Vec2 vDir = pMon->GetPos() - pPlayer->GetPos();

    if (pMon->GetMstInfo().m_iHP < 1)
    {
        ChangeState(L"Dead");
        m_bNowAttack   = false;
        m_fAttackDelay = 0.f;
    }
    else if (GetCQC())
    {
        if (m_bNowAttack)
        {
            // 공격상태 중이면 공격 딜레이만큼 시간이 지난 뒤 공격상태를 해제함
            m_fAttackDelay += DT;
            if (m_fAttackDelay > fAttackDelay)
            {
                m_bNowAttack   = false;
                m_fAttackDelay = 0.f;
                ChangeState(L"Trace");
            }
        }
        else if (vDir.Length() < fAttackRange * 0.2f)
        {
            ChangeState(L"AttackCQC");
        }
        else if (vDir.Length() < fAttackRange * 0.5f)
        {
            ChangeState(L"TraceSly");
        }
        else if (vDir.Length() < fAttackRange)
        {
            m_bNowAttack = true;
        }
        else if (vDir.Length() > fAttackRange)
        {
            ChangeState(L"Trace");
        }
    }
    else if (!GetCQC())
    {
        if (m_bNowAttack)
        {
            // 공격상태 중이면 공격 딜레이만큼 시간이 지난 뒤 공격상태를 해제함
            m_fAttackDelay += DT;
            if (m_fAttackDelay > fAttackDelay)
            {
                m_bNowAttack   = false;
                m_fAttackDelay = 0.f;
                ChangeState(L"Trace");
            }
        }
        else if (vDir.Length() < fAttackRange)
        {
            m_bNowAttack = true;
        }
        else if (vDir.Length() > fAttackRange)
        {
            ChangeState(L"Trace");
        }
    }
}

void CAttack::Enter()
{
}

void CAttack::Exit()
{
}
