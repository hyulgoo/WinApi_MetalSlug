#include "pch.h"
#include "CCQCArea.h"
#include "CTimeMgr.h"
#include "CCollider.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CPlayer.h"
#include "CEngine.h"


CCQCArea::CCQCArea()
	: m_bCQC(false)
	, m_fTime(0.f)
{
	CreateCollider();
	SetPos(Vec2(GetPos().x + 40, GetPos().y));
	GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
	SetScale(Vec2(120.f, 120.f));
}

CCQCArea::~CCQCArea()
{
}

void CCQCArea::tick()
{
	CObj::tick();
}

void CCQCArea::render(HDC _dc)
{
	CObj::render(_dc);
}

void CCQCArea::OnOverlap(CCollider* _pOther)
{
	m_bCQC = true;
}

void CCQCArea::EndOverlap(CCollider* _pOther)
{
	m_bCQC = false;
}
