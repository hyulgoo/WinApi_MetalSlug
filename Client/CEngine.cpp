#include "pch.h"
#include "CEngine.h"

#include "CEventMgr.h"
#include "CPathMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CLevelMgr.h"
#include "CCollisionMgr.h"
#include "CCamera.h"
#include "CResMgr.h"
#include "CUIMgr.h"

#include "CTexture.h"


CEngine::CEngine()
	: m_hMainWnd(nullptr)
	, m_hDC(nullptr)
	, m_ptResolution{}
	, m_arrPen{}
	, m_pMemTex(nullptr)
	, m_BackGroundTex(nullptr)
{	
}

CEngine::~CEngine()
{
	ReleaseDC(m_hMainWnd, m_hDC);

	for (UINT i = 0; i < (UINT)PEN_TYPE::END; ++i)
	{
		DeleteObject(m_arrPen[i]);
	}
}

void CEngine::init(HWND _hwnd, UINT _iWidth, UINT _iHeight)
{
	m_hMainWnd = _hwnd;
	m_ptResolution.x = _iWidth;
	m_ptResolution.y = _iHeight;

	// HDC 초기화
	m_hDC = GetDC(m_hMainWnd);

	// 윈도우 크기 설정
	ChangeWindowSize(_iWidth, _iHeight);
	// 백버퍼용 비트맵 제작
	// m_pMemTex = CResMgr::GetInst()->GetInst()->CreateTexture(L"BackBuffer", m_ptResolution.x, m_ptResolution.y);
	
	// 자주 사용하는 Pen 및 Brush 생성해둠
	CreatePenBrush();

	// Manager 초기화
	CPathMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CKeyMgr::GetInst()->init();
	CLevelMgr::GetInst()->init();	
}

void CEngine::progress()
{
	// 논리적인 연산
	tick();

	// 화면 렌더링 갱신
	render();

	// 다음 프레임에 적용될 이벤트 처리
	CEventMgr::GetInst()->tick();
}

void CEngine::tick()
{
	// FPS, DeltaTime 계산
	CTimeMgr::GetInst()->tick();

	// Key Event 체크
	CKeyMgr::GetInst()->tick();

	// Camera
	CCamera::GetInst()->tick();
	
	// Level 을 업데이트
	CLevelMgr::GetInst()->tick();

	// 변경점 기반, 충돌체크
	CCollisionMgr::GetInst()->tick();

	// UI 클릭 체크
	CUIMgr::GetInst()->tick();

}

void CEngine::render()
{
	// 화면 클리어
	Rectangle(m_pMemTex->GetDC(), -1, -1, m_ptResolution.x + 1, m_ptResolution.y + 1);

	// 레벨 렌더
	CLevelMgr::GetInst()->render(m_pMemTex->GetDC());

	// 카메라 블라인드
	CCamera::GetInst()->render(m_pMemTex->GetDC());

	// MemBitMap -> MainWindowBitmap
	BitBlt(m_hDC, 0, 0, m_ptResolution.x, m_ptResolution.y, m_pMemTex->GetDC(), 0, 0, SRCCOPY);

	CTimeMgr::GetInst()->render();
}

void CEngine::CreatePenBrush()
{
	m_arrPen[(UINT)PEN_TYPE::RED] = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_arrPen[(UINT)PEN_TYPE::GREEN] = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	m_arrPen[(UINT)PEN_TYPE::BLUE] = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_arrPen[(UINT)PEN_TYPE::WHITE] = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	m_arrPen[(UINT)PEN_TYPE::NONE] = CreatePen(PS_NULL, 1, RGB(255, 255, 255));
}
	
void CEngine::ChangeWindowSize(UINT _width, UINT _height)
{
	m_ptResolution.x = _width;
	m_ptResolution.y = _height;

	// RECT = 좌상단 위치, 가로, 세로 길이를 같는 해상도 단위
	RECT rt = { 0, 0, m_ptResolution.x , m_ptResolution.y };

	// 현재 메인 윈도우에서 메뉴를 얻어옴
	HMENU hMenu = GetMenu(m_hMainWnd);
	
	// 있다면 있는 기준으로 해상도 설정, 없다면 없는 기준으로 설정
	if (nullptr != hMenu)
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, true);
	else
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// 백버퍼가 없으면 생성 있다면 변경된 해상도에 맞춰 조정
	if (nullptr == m_pMemTex)
		m_pMemTex = CResMgr::GetInst()->CreateTexture(L"BackBuffer", _width, _height);
	else
		m_pMemTex->Resize(m_ptResolution.x, m_ptResolution.y);
}

HDC CEngine::GetMemTexDC()
{
	return m_pMemTex->GetDC(); 
}
