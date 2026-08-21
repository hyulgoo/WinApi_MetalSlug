#include "pch.h"
#include "CGround.h"

#include "CCollider.h"
#include "CPlayer.h"
#include "CRigidbody.h"
CGround::CGround()
{
	CreateCollider();
}

CGround::~CGround()
{
}

void CGround::BeginOverlap(CCollider* _pOther)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pOther->GetOwner());
	if (nullptr == pPlayer)
		return;

	pPlayer->GetRigidbody()->SetGround(true);
}

void CGround::OnOverlap(CCollider* _pOther)
{
}

void CGround::EndOverlap(CCollider* _pOther)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pOther->GetOwner());
	if (nullptr == pPlayer)
		return;

	pPlayer->GetRigidbody()->SetGround(false);
}
