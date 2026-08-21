#include "pch.h"
#include "CComponent.h"

CComponent::CComponent(CObj* _pOwner)
	: m_pOwner(_pOwner)
{
}

CComponent::~CComponent()
{
}
