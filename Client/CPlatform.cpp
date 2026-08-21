#include "pch.h"
#include "CPlatform.h"

#include "CCollider.h"
#include "CRigidbody.h"
#include "CPlayer.h"

CPlatform::CPlatform()
{
	CreateCollider();
	GetCollider()->SetScale(Vec2(400.f,50.f));
}

CPlatform::~CPlatform()
{
}

void CPlatform::BeginOverlap(CCollider* _pOther)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pOther->GetOwner());
	if (nullptr == pPlayer)
		return;

	pPlayer->GetRigidbody()->SetGround(true);
}

void CPlatform::OnOverlap(CCollider* _pOther)
{
}

void CPlatform::EndOverlap(CCollider* _pOther)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pOther->GetOwner());
	if (nullptr == pPlayer)
		return;

	pPlayer->GetRigidbody()->SetGround(false);
}

