#include "pch.h"
#include "CPixelCollider.h"
#include "CResMgr.h"
#include "CEngine.h"
#include "CTexture.h"
#include "CObj.h"
#include "CRigidbody.h"
#include "CTimeMgr.h"

#include "CKeyMgr.h"

CPixelCollider::CPixelCollider(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_pTex(nullptr)
	, m_fmagni(4.f)
	, m_bPlayPixel(false)
	, m_bJump(false)
	, m_bGround(false)
	, m_bWall(false)
	, m_bGravity(true)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_OwnerPos = GetOwner()->GetPos();
	m_vFinalPos = m_OwnerPos + m_vOffsetPos;

	m_pAtlas = CResMgr::GetInst()->LoadTexture(L"PixelBackGroundasd", L"texture\\PixelBackGround.bmp");
	m_pTex = CResMgr::GetInst()->CreateTexture(L"PixelBackGround_BackBuffer", (UINT)(m_vResolution.x + 400), (UINT)(m_vResolution.y + 400));
}

CPixelCollider::~CPixelCollider()
{
}

void CPixelCollider::tick()
{
	Vec2 vPos = Vec2(0.f, 0.f);
	vPos = CCamera::GetInst()->GetRealPos(vPos);\
	//if (IsPressed(KEY::LEFT))
	//{
	//	vPos -= Vec2(300.f * DT, 0.f);
	//}
	//if (IsPressed(KEY::RIGHT))
	//{
	//	vPos += Vec2(300.f * DT, 0.f);
	//}
	if(m_bGravity)
	GetOwner()->SetPos(GetOwner()->GetPos() + Vec2(0.f, 300.f * DT));
	

	StretchBlt(m_pTex->GetDC()
		, 0, 0
		, (int)(m_vResolution.x + 400), (int)(m_vResolution.y + 200)
		, m_pAtlas->GetDC()
		, (int)(vPos.x)
		, (int)(vPos.y)
		, (int)((m_vResolution.x + 400) / m_fmagni), (int)((m_vResolution.y + 200) / m_fmagni)
		, SRCCOPY);


	m_OwnerScale = GetOwner()->GetScale();
	m_OwnerPos = GetOwner()->GetPos();
	m_vFinalPos = m_OwnerPos + m_vOffsetPos;
	m_vGroundPoint1 = Vec2((m_vFinalPos.x), (m_vFinalPos.y + m_OwnerScale.y / 2));
	m_vGroundPoint2 = Vec2(m_vGroundPoint1.x, m_vGroundPoint1.y - 1);
	m_vWallPoint1 = Vec2((m_vFinalPos.x - m_OwnerScale.x / 2), (m_vFinalPos.y));
	m_vWallPoint2 = Vec2(m_vWallPoint1.x - 1,m_vWallPoint1.y);

	COLORREF GroundCheck1, GroundCheck2, WallPoint1, WallPoint2, JumpPoint = {};
	GroundCheck1 = GetPixel(m_pTex->GetDC(), (int)m_vGroundPoint1.x, (int)m_vGroundPoint1.y);
	GroundCheck2 = GetPixel(m_pTex->GetDC(), (int)m_vGroundPoint2.x, (int)m_vGroundPoint2.y);
	WallPoint1 = GetPixel(m_pTex->GetDC(), (int)m_vWallPoint1.x, (int)m_vWallPoint1.y);
	WallPoint2 = GetPixel(m_pTex->GetDC(), (int)m_vWallPoint2.x, (int)m_vWallPoint2.y);
	JumpPoint = GetPixel(m_pTex->GetDC(), (int)m_vGroundPoint1.x, (int)m_vGroundPoint1.y + 10);
	COLORREF magenta = RGB(255, 0, 255);
	COLORREF green = RGB(0, 255, 0);

	if (JumpPoint == magenta)
		m_bJump = false;
	else if (JumpPoint != magenta)
		m_bJump = true;
	
	if (WallPoint1 == green)
	{
		m_bWall = true;
		m_bGround = false;
		// 땅 위에 있는 상태로 만들고
		while (WallPoint2 == green)
		{	// 체크한 바로 옆에 픽셀을 체크하고 옆이 연두색이 아닐 때까지 오른쪽으로 밈
			m_vWallPoint2.x += 1.f;
			GetOwner()->SetPos(Vec2(m_vWallPoint2.x + m_OwnerScale.x/2 + 1, m_vWallPoint2.y));
			WallPoint2 = GetPixel(m_pTex->GetDC(), (int)m_vWallPoint2.x, (int)m_vWallPoint2.y);
		}
	}
		
	else if (GroundCheck1 == magenta)
	{
			// 땅 위에 있는 상태로 만들고
		m_bGround = true;
		m_bWall = false;

		GetOwner()->GetRigidbody()->SetGround(true);
		while (GroundCheck2 == magenta)
		{	// 바로 위에 픽셀을 체크하고 아래는 마젠타 위는 다른색일 때까지 올림
			m_vGroundPoint2.y -= 1.f;
			GetOwner()->SetPos(Vec2(m_vGroundPoint2.x, (m_vGroundPoint2.y - m_OwnerScale.y / 2) + 1));
			GroundCheck2 = GetPixel(m_pTex->GetDC(), (int)m_vGroundPoint2.x, (int)m_vGroundPoint2.y);
		}
	}

	else
	{
		GetOwner()->SetPos(m_vFinalPos);
		GetOwner()->GetRigidbody()->SetGround(false);
		m_bGround = false;
		m_bWall = false;
	}
}

void CPixelCollider::final_tick()
{
}


void CPixelCollider::render(HDC _dc)
{
	if (m_bPlayPixel)
	{
		StretchBlt(_dc
			, 0, 0
			, (int)(m_vResolution.x), (int)(m_vResolution.y)
			, m_pTex->GetDC()
			, 0
			, 0
			, (int)(m_vResolution.x), (int)(m_vResolution.y)
			, SRCCOPY);
	}
	

	//HPEN hPen = nullptr;
	//hPen = CEngine::GetInst()->GetPen(PEN_TYPE::WHITE);
	//HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	//
	//// DC 의 기존 펜과 브러시를 새로 가져온 것들로 대체한다
	//HPEN hOriginPen = (HPEN)SelectObject(_dc, hPen);
	//HBRUSH hOriginBrush = (HBRUSH)SelectObject(_dc, hNullBrush);
	//
	//// 사각형 그리기
	//Rectangle(_dc, (int)(m_vGroundPoint2.x - 2)
	//	, (int)(m_vGroundPoint2.y -1)
	//	, (int)(m_vGroundPoint2.x + 2)
	//	, (int)(m_vGroundPoint2.y + 1));
	//
	//Rectangle(_dc, (int)(m_vWallPoint1.x - 1)
	//	, (int)(m_vWallPoint1.y - 2)
	//	, (int)(m_vWallPoint1.x + 1)
	//	, (int)(m_vWallPoint1.y + 2));
	//
	//// DC 의 GDI 오브젝트들을 기존의 펜과 브러시로 되돌린다.
	//SelectObject(_dc, hOriginPen);
	//SelectObject(_dc, hOriginBrush);
}
