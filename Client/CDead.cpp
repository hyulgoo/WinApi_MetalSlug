#include "pch.h"
#include "CDead.h"

#include "CTimeMgr.h"

#include "CMonster.h"

void CDead::final_tick()
{
	CMonster* pMon = dynamic_cast<CMonster*>(GetOwnerObj());
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
