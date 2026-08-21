#include "pch.h"

#include "framework.h"
#include "Client.h"

#include "CEngine.h"

#include <vector>
using std::vector;

#define MAX_LOADSTRING 100


// App Title
const wchar_t* g_pTitle = L"MyGame";
HINSTANCE   hInst;                             
HWND        g_hWnd; // 메인 윈도우 핸들

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT __stdcall   WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR __stdcall   About(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK CreateAnimation(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);



int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(4430);
    // 윈도우, DC, Brush, Pen, AccelTable 등등
    // Handle 로 다루는 오브젝트들은 kernel Object ( OS 가 관리하는 오브젝트 )
    // 윈도우 설정 세팅
    MyRegisterClass(hInstance);
  
    // 윈도우 생성
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // 엔진 초기화
    CEngine::GetInst()->init(g_hWnd, 1200, 800);


    // 단축키 테이블 정보 가져옴
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));
    MSG msg = {};

    // GetMessage : 메세지 큐에서 메세지를 꺼내옴
    // 리턴값 BOOL false 인 경우 프로그램이 종료된다.
    // 꺼내온 message 가 WM_QUIT 이면 GetMessage 는 false 를 반환한다.

    // PeekMessage : Peek(훔쳐 보터, 흘깃 보다)
    // 메세지 큐에 메세지가 있든 없든 무조건 함수가 반환된다.
    // PM_REMOVE 옵션과 조합할 경우, 메세지가 있으면 메세지 큐에서 메시지를 꺼내온다 ( GetMessage 랑 동일해짐)
    // 리턴값 BOOL 이 true 인 경우 메세지를 꺼내왔다, false 인 경우 메세지가 없었다.

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            // 단축키 조합 확인
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg); // 분석
                DispatchMessage(&msg);  // 처리
            }
        }

        else
        {
            // Game Run
            CEngine::GetInst()->progress();
        }      
    }
    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.lpszClassName  = L"MySetting";
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));    

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   g_hWnd = CreateWindowW(L"MySetting", g_pTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, true);
   UpdateWindow(g_hWnd);

   return TRUE;
}



// 메인 윈도우의 메세지 처리기 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                // Modal 방식 : 해당 창을 띄우면 다른 윈도우에서 반응할 수 없음.
                // peekmassage에서 해당 창을 띄우고 반응을 기다리고 있으면
                // 해당 창이 종료될 때까지porgress가 실행되지 않기 때문
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case CREATEANIMATION:
            {
                int iReturn = DialogBox(hInst, MAKEINTRESOURCE(IDD_TILECOUNT), hWnd, CreateAnimation);
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
              

        // WM_PAINT
        // 윈도우에 Rendering 이 다시 발생해야 할 때,
        // 해당 윈도우에 무효화 영역이 생겼을 때
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            
            // Handle Device Context
            // Redering 에 필요한 데이터 DC
            HDC hdc = BeginPaint(hWnd, &ps);
          

            // 그리기 종료
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
