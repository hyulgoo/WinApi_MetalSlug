#include "pch.h"
#include "Game\04.Object\Character\Monster\Boss\CBoss.h"

#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

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

void CBoss::LoadAnimation() const
{
}

void CBoss::PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight) const
{
}

void CBoss::Play(bool _bDir) const
{
}
