#include "pch.h"
#include "CForce.h"

#include "CTimeMgr.h"
#include "CEngine.h"

#include "CRigidbody.h"

#include "CLevelMgr.h"
#include "CLevel.h"
CForce::CForce()
	: m_fAccTime(0.f)
	, m_fForce(0.f)
	, m_fRadius(0.f)
	, m_fLifeTime(0.f)
{
}

CForce::~CForce()
{
}

void CForce::tick()
{
	m_fAccTime += DT;
	if (m_fLifeTime < m_fAccTime)
	{
		SetDead();
		m_fAccTime = 0.f;
	}

	// Player Layer에 접근하여 Layer Object 배열를 가져옴
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	const vector<CObj*>& vecPlayerLayer = pCurLevel->GetLayer(LAYER::PLAYER);

	// 오브젝트의 Rigidbody가 없다면 건너뜀.
	for (size_t i = 0; i < vecPlayerLayer.size(); ++i)
	{
		if (nullptr == vecPlayerLayer[i]->GetRigidbody())
			continue;

		Vec2 vForceDir = vecPlayerLayer[i]->GetPos() - GetPos();

		float fDistance = vForceDir.Length();

		float fRatio = 1.f - fDistance / m_fRadius;
		Saturate(fRatio);

		vForceDir.Normalize();
		vForceDir *= m_fForce * fRatio;

		vecPlayerLayer[i]->GetRigidbody()->AddForce(vForceDir);
	}

	const vector<CObj*>& vecMonsterLayer = pCurLevel->GetLayer(LAYER::MONSTER);

	// 오브젝트의 Rigidbody가 없다면 건너뜀.
	for (size_t i = 0; i < vecMonsterLayer.size(); ++i)
	{
		if (nullptr == vecMonsterLayer[i]->GetRigidbody())
			continue;

		Vec2 ForceDir = vecMonsterLayer[i]->GetPos() - GetPos();

		float Distance = ForceDir.Length();

		float Ratio = 1.f - Distance / m_fRadius;
		Saturate(Ratio);

		ForceDir.Normalize();
		ForceDir *= m_fForce * Ratio;

		vecMonsterLayer[i]->GetRigidbody()->AddForce(ForceDir);
	}

}

void CForce::render(HDC _dc)
{
	HPEN hGreenPen = CEngine::GetInst()->GetPen(PEN_TYPE::GREEN);
	HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
	HPEN hOriginPen = (HPEN)SelectObject(_dc, hGreenPen);
	HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);

	Vec2 vPos = CCamera::GetInst()->GetRenderPos(GetPos());

	// 사각형 그리기
	Ellipse(_dc
		, (int)(vPos.x - m_fRadius)
		, (int)(vPos.y - m_fRadius)
		, (int)(vPos.x + m_fRadius)
		, (int)(vPos.y + m_fRadius));

	// DC 의 GDI 오브젝트들을 기존의 펜과 브러시로 되돌린다.
	SelectObject(_dc, hOriginPen);
	SelectObject(_dc, hOriginBrush);
}
