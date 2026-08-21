#include "pch.h"
#include "CHandgun.h"

#include "CTimeMgr.h"
#include "CCollider.h"
#include "CPixelCollider.h"
#include "CRigidbody.h"
#include "CWeaponeEffect.h"

#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CTexture.h"


CHandgun::CHandgun()
	: m_fSpeed(200.f)
	, m_fDegree(80.f)
	, m_fTime(0.f)
	, m_pTexture(nullptr)
	, m_fmagni(3.3f)
{
	CreateCollider();
	CreatePixelCollider();
	CreateRigidbody();
	GetRigidbody()->SetGravity(false);
	GetPixelCollider()->SetGravity();
	SetScale(Vec2(1.f, 1.f));
	GetCollider()->SetOffsetPos(Vec2(17.f, 10.f));
	SetSpeed(1300.f);
	m_pTexture = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"BulletTexture", 50, 50);
}
CHandgun::~CHandgun()
{
}

void CHandgun::tick()
{
	if (CCamera::GetInst()->IsCameraStop() == false)
		SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

	Vec2 vPos = GetPos();	
	// 60분법 -> 라디안
	float fRadian = (m_fDegree * PI) / 180.f;

	vPos.x += m_fSpeed * cosf(fRadian) * DT;
	vPos.y -= m_fSpeed * sinf(fRadian) * DT;

	SetPos(vPos);
	if (GetRigidbody()->IsGround())
	{
		CWeaponeEffect* effect = new CWeaponeEffect;
		effect->SetState(0);
		Instantiate(effect, GetPos(), LAYER::EFFECT);
		if(!IsDead())
			SetDead();
	}
	else if (GetPixelCollider()->IsWall())
	{
		CWeaponeEffect* effect = new CWeaponeEffect;
		effect->SetState(2);
		Instantiate(effect, GetPos(), LAYER::EFFECT);
		if (!IsDead())
			SetDead();

	}
	// 3초가 지나면 스스로 삭제
	m_fTime += DT;
	if (0.75f < m_fTime && !IsDead())
	{
		SetDead();
	}

	CObj::tick();
}

void CHandgun::render(HDC _dc)
{
	Vec2 vPos = GetPos();
	Vec2 vSize = {};
	Vec2 vLeftTop = {};
	if (m_bVRT)
	{
		vSize = Vec2(6.f, 10.f);
		vLeftTop = Vec2(78.f, 3.f);
		GetCollider()->SetOffsetPos(Vec2(10.f, 10.f));
	}
	else
	{
		vSize = Vec2(10.f, 6.f);
		vLeftTop = Vec2(4.f, 5.f);
	}
	StretchBlt(m_pBackBuffer->GetDC()
		, 0, 0
		, int(vSize.x * m_fmagni), int(vSize.y * m_fmagni)
		, m_pTexture->GetDC()
		, int(vLeftTop.x)
		, int(vLeftTop.y)
		, int(vSize.x)
		, int(vSize.y), SRCCOPY);
	TransparentBlt(_dc
		, (int)vPos.x
		, (int)vPos.y
		, int(vSize.x * m_fmagni), int(vSize.y * m_fmagni)
		, m_pBackBuffer->GetDC()
		, 0, 0
		, int(vSize.x * m_fmagni), int(vSize.y * m_fmagni)
		, RGB(255, 0, 255));

	CObj::render(_dc);
}

void CHandgun::BeginOverlap(CCollider* _pOther)
{
	if (_pOther->GetOwner()->GetLayer() == LAYER::MONSTER)
	{
		CWeaponeEffect* effect = new CWeaponeEffect;
		effect->SetState(1);
		Instantiate(effect, GetPos(), LAYER::EFFECT);
		if (!IsDead())
			SetDead();
	}
}
