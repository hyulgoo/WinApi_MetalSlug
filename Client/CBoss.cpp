#include "pch.h"
#include "CBoss.h"

#include "CAI.h"
#include "CState.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CCollider.h"

#include "CDeadEffect.h"
CBoss::CBoss()
{
}

CBoss::~CBoss()
{
}

void CBoss::tick()
{
}

void CBoss::render(HDC _dc)
{
}

void CBoss::BeginOverlap(CCollider* _pOther)
{
}

void CBoss::OnOverlap(CCollider* _pOther)
{
}

void CBoss::LoadAnimation()
{
}

void CBoss::PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight)
{
}

void CBoss::Play(bool _bDir)
{
}
