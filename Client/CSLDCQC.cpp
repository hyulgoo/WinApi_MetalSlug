#include "pch.h"
#include "CSLDCQC.h"
#include "CTimeMgr.h"
#include "CCollider.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"


void CSLDCQC::BeginOverlap(CCollider* _pOther)
{
}

CSLDCQC::CSLDCQC()
	: m_fTime(0.f)
	, m_bStart(false)
	, m_fOffset()
	, m_bDir(false)
{
	SetScale(Vec2(40.f, 40.f));
	CreateCollider();
}

CSLDCQC::~CSLDCQC()
{
}

void CSLDCQC::tick()
{
	if (m_bStart == false)
	{
		if (m_bDir)
			m_fOffset = 40.f;
		else 
			m_fOffset = -40.f;
		SetPos(Vec2(GetPos().x + m_fOffset, GetPos().y));
		m_bStart = true;
	}

	m_fTime += DT;
	if (0.001f < m_fTime && !IsDead())
	{
		SetDead();
	}

	CObj::tick();
}

void CSLDCQC::render(HDC _dc)
{

	CObj::render(_dc);
}
