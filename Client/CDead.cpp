#include "pch.h"
#include "CDead.h"

#include "CMonster.h"

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
