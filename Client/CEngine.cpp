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

	for (auto& i : m_arrPen)
    {
		DeleteObject(i);
	}
}

void CEngine::init(const HWND _hwnd, const UINT _iWidth, const UINT _iHeight)
{
	m_hMainWnd = _hwnd;
	m_ptResolution.x = _iWidth;
	m_ptResolution.y = _iHeight;

	// HDC 초기화
	m_hDC = GetDC(m_hMainWnd);

	// 윈도우 크기 설정
	ChangeWindowSize(_iWidth, _iHeight);
	// 백버퍼용 텍스처 생성
	// m_pMemTex = CResMgr::GetInst()->GetInst()->CreateTexture(L"BackBuffer", m_ptResolution.x, m_ptResolution.y);

	// 앞으로 사용할 Pen 과 Brush 미리 만들어둠
	CreatePenBrush();

	// Manager 초기화
	CPathMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CKeyMgr::GetInst()->init();
	CLevelMgr::GetInst()->init();	
}

void CEngine::progress() const
{
	// 로직 갱신
	tick();

	// 화면 렌더링 처리
	render();

	// 다음 프레임에 사용할 이벤트 처리
	CEventMgr::GetInst()->tick();
}

void CEngine::tick() const
{
	// FPS, DeltaTime 계산
	CTimeMgr::GetInst()->tick();

	// Key Event 체크
	CKeyMgr::GetInst()->tick();

	// Camera
	CCamera::GetInst()->tick();

	// Level 업데이트
	CLevelMgr::GetInst()->tick();

	// 레이어 간 충돌체크
	CCollisionMgr::GetInst()->tick();

	// UI 클릭 체크
	CUIMgr::GetInst()->tick();
}

void CEngine::render() const
{
	// 화면 클리어
	Rectangle(m_pMemTex->GetDC(), -1, -1, m_ptResolution.x + 1, m_ptResolution.y + 1);

	// 레벨 렌더링
	CLevelMgr::GetInst()->render(m_pMemTex->GetDC());

	// 카메라 렌더링
	CCamera::GetInst()->render(m_pMemTex->GetDC());

	// MemBitMap -> MainWindowBitmap
	BitBlt(m_hDC, 0, 0, m_ptResolution.x, m_ptResolution.y, m_pMemTex->GetDC(), 0, 0, SRCCOPY);

	CTimeMgr::GetInst()->render();
}

void CEngine::CreatePenBrush()
{
	m_arrPen[static_cast<UINT>(PEN_TYPE::RED)]   = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_arrPen[static_cast<UINT>(PEN_TYPE::GREEN)] = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	m_arrPen[static_cast<UINT>(PEN_TYPE::BLUE)]  = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_arrPen[static_cast<UINT>(PEN_TYPE::WHITE)] = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	m_arrPen[static_cast<UINT>(PEN_TYPE::NONE)]  = CreatePen(PS_NULL, 1, RGB(255, 255, 255));
}
	
void CEngine::ChangeWindowSize(const UINT _width, const UINT _height)
{
	m_ptResolution.x = _width;
	m_ptResolution.y = _height;

	// RECT = 좌상단 위치, 가로, 세로 길이로 만든 해상도 영역
	RECT rt = { 0, 0, m_ptResolution.x , m_ptResolution.y };

	// 현재 실행중인 윈도우에 메뉴가 있는지 확인
	const HMENU hMenu = GetMenu(m_hMainWnd);

	// 있다면 있는 기준으로 해상도 설정, 없다면 없는 기준으로 설정
	if (nullptr != hMenu)
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, true);
	else
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// 백버퍼가 아직 없다면 새로 만들고, 있다면 새 해상도에 맞게 리사이즈
	if (nullptr == m_pMemTex)
		m_pMemTex = CResMgr::GetInst()->CreateTexture(L"BackBuffer", _width, _height);
	else
		m_pMemTex->Resize(m_ptResolution.x, m_ptResolution.y);
}

HDC CEngine::GetMemTexDC() const
{
	return m_pMemTex->GetDC(); 
}
