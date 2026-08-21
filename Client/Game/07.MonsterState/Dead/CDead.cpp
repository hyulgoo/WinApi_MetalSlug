#include "pch.h"
#include "Game\07.MonsterState\Dead\CDead.h"

#include "Game\04.Object\Character\Monster\CMonster.h"

void CDead::final_tick()
{
    const CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
    assert(pMon);
}

void CDead::Enter()
{
}

void CDead::Exit()
{
}

CDead::CDead()
{
}

CDead::~CDead()
{
}
