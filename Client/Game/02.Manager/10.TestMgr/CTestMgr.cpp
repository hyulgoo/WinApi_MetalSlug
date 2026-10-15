#include "pch.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

#include <csignal>
#include <cstdarg>
#include <cmath>
#include <cwctype>
#include <typeinfo>
#include <shellapi.h>
#include <psapi.h>
#include <dbghelp.h>

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\04.CollisionMgr\CCollisionMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\03.Level\02.Stage\CStage.h"
#include "Game\04.Object\CObj.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\04.Object\Character\Monster\CMonster.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

#include <algorithm>
#include <limits>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Dbghelp.lib")

namespace
{
    // KEY 열거 순서와 같아야 한다 (CKeyMgr.h)
    const wchar_t* g_arrKeyName[static_cast<UINT>(KEY::END)] =
    {
        L"UP", L"DOWN", L"LEFT", L"RIGHT",
        L"SPACE", L"ENTER", L"ESC", L"LALT", L"LCTRL", L"LSHIFT", L"TAB",
        L"LBTN", L"RBTN",
        L"Q", L"W", L"E", L"R",
        L"A", L"S", L"D", L"F",
        L"I", L"K", L"J", L"L", L"T",
        L"Z", L"X", L"C", L"V",
        L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8",
        L"F1", L"F2", L"F3", L"F4", L"F5", L"F6",
    };

    bool StrToKey(const wstring& _strName, UINT& _iKey)
    {
        for (UINT i = 0; i < static_cast<UINT>(KEY::END); ++i)
        {
            if (_strName == g_arrKeyName[i])
            {
                _iKey = i;
                return true;
            }
        }
        return false;
    }

    bool StrToMonster(const wstring& _strName, MONSTER& _eType)
    {
        const wchar_t* arrName[] = { L"SOLDIER", L"ZB_GIRL", L"ZB_BOY", L"ZB_MAN", L"ZB_OLD", L"ZB_RSH" };
        for (UINT i = 0; i < static_cast<UINT>(MONSTER::END); ++i)
        {
            if (_strName == arrName[i])
            {
                _eType = static_cast<MONSTER>(i);
                return true;
            }
        }
        return false;
    }

    // typeid 이름에서 "class " 를 뗀 클래스 이름. 좀비는 한 클래스(CZombie)라 종류 이름(예전 클래스 이름)으로 부른다
    wstring ClassName(CObj* _pObj)
    {
        if (const CZombie* pZombie = dynamic_cast<const CZombie*>(_pObj))
            return pZombie->GetDesc().szTestName;

        const char* szName = typeid(*_pObj).name();
        if (0 == strncmp(szName, "class ", 6))
            szName += 6;

        wstring strName;
        for (const char* p = szName; *p; ++p)
            strName += static_cast<wchar_t>(*p);
        return strName;
    }

    vector<wstring> Split(const wstring& _strLine)
    {
        vector<wstring> vecToken;
        wstring         strCur;
        for (const wchar_t ch : _strLine)
        {
            if (iswspace(ch))
            {
                if (!strCur.empty())
                    vecToken.push_back(strCur);
                strCur.clear();
            }
            else
            {
                strCur += ch;
            }
        }
        if (!strCur.empty())
            vecToken.push_back(strCur);
        return vecToken;
    }

    bool ToInt(const wstring& _str, int& _iOut)
    {
        wchar_t* pEnd = nullptr;
        const long lValue = wcstol(_str.c_str(), &pEnd, 10);
        if (pEnd == _str.c_str() || *pEnd != 0)
            return false;
        _iOut = static_cast<int>(lValue);
        return true;
    }

    bool ToDouble(const wstring& _str, double& _dOut)
    {
        wchar_t* pEnd = nullptr;
        _dOut = wcstod(_str.c_str(), &pEnd);
        return !(pEnd == _str.c_str() || *pEnd != 0);
    }

    CPlayer* FindPlayer()
    {
        CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
        if (nullptr == pLevel)
            return nullptr;

        for (CObj* pObj : pLevel->GetLayer(LAYER::PLAYER))
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);
            if (nullptr != pPlayer)
                return pPlayer;
        }
        return nullptr;
    }

    // 화면에 보이는 위치. 로그는 좌표계와 상관없이 화면 기준으로 비교한다
    Vec2 ScreenPos(CObj* _pObj)
    {
        return _pObj->GetScreenPos();
    }

    // 월드 위치(화면 픽셀 배율, 배경 텍스처 1px = BG_MAGNI). 오브젝트가 배경에 붙어 있는지 볼 때 쓴다
    Vec2 WorldPos(CObj* _pObj)
    {
        return _pObj->GetPos();
    }

    // 카메라 왼쪽 위. 스테이지는 레벨 파일과 같은 배경 텍스처 좌표로 (카메라는 월드 좌표다)
    Vec2 CameraLT()
    {
        Vec2 vLT = CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f));
        if (nullptr != dynamic_cast<CStage*>(CLevelMgr::GetInst()->GetCurLevel()))
            vLT = vLT / BG_MAGNI;
        return vLT;
    }

    const wchar_t* AnimName(const CAnimator* _pAnimator)
    {
        if (nullptr == _pAnimator || nullptr == _pAnimator->GetAnimation())
            return L"-";
        return _pAnimator->GetAnimation()->GetName().c_str();
    }

    // 충돌체 위치가 오너 위치(+오프셋)에서 떨어진 거리. 충돌 판정이 이번 프레임 위치로 되는지 본다. 충돌체가 없으면 -1
    double ColliderLag(CObj* _pObj)
    {
        CCollider* pCol = _pObj->GetCollider();
        if (nullptr == pCol)
            return -1.0;
        const Vec2 vDiff = pCol->GetFinalPos() - (_pObj->GetPos() + pCol->GetOffsetPos());
        return sqrt(static_cast<double>(vDiff.x) * vDiff.x + static_cast<double>(vDiff.y) * vDiff.y);
    }

    // 재생 중인 애니메이션의 프레임 번호. 없으면 -1
    int AnimFrame(const CAnimator* _pAnimator)
    {
        if (nullptr == _pAnimator || nullptr == _pAnimator->GetAnimation())
            return -1;
        return _pAnimator->GetAnimation()->GetCurFrmNumber();
    }

    long long GdiCount()
    {
        return static_cast<long long>(GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS));
    }

    long long PrivateBytes()
    {
        PROCESS_MEMORY_COUNTERS_EX tInfo = {};
        tInfo.cb = sizeof(tInfo);
        if (!GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&tInfo), sizeof(tInfo)))
            return 0;
        return static_cast<long long>(tInfo.PrivateUsage);
    }

    // CRT 보고(누수·assert·힙 오류)를 받을 파일. CRT 종료 뒤 누수 보고까지 받아야 하므로 닫지 않는다
    HANDLE g_hCrtFile = INVALID_HANDLE_VALUE;

    void WriteCrtFile(const char* _szText)
    {
        if (INVALID_HANDLE_VALUE == g_hCrtFile)
            return;
        DWORD dwWritten = 0;
        WriteFile(g_hCrtFile, _szText, static_cast<DWORD>(strlen(_szText)), &dwWritten, nullptr);
    }

#ifdef _DEBUG
    int __cdecl CrtReportHook(const int _iReportType, char* _szMessage, int* _pRetVal)
    {
        WriteCrtFile(_szMessage);
        if (nullptr != _pRetVal)
            *_pRetVal = 0;

        // 누수 보고(_CRT_WARN)는 기록만 하고, assert·힙 오류는 바로 실패로 끝낸다
        if (_CRT_WARN != _iReportType)
        {
            wchar_t szMsg[1024] = {};
            MultiByteToWideChar(CP_UTF8, 0, _szMessage, -1, szMsg, 1023);
            CTestMgr::GetInst()->Abort(CTestMgr::EXIT_CRT_ERROR, szMsg);
        }
        return TRUE;
    }
#endif

    void WriteStackTrace(CONTEXT* _pContext)
    {
        HANDLE hProcess = GetCurrentProcess();
        HANDLE hThread  = GetCurrentThread();
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
        SymInitialize(hProcess, nullptr, TRUE);

        STACKFRAME64 tFrame      = {};
        tFrame.AddrPC.Offset     = _pContext->Rip;
        tFrame.AddrPC.Mode       = AddrModeFlat;
        tFrame.AddrFrame.Offset  = _pContext->Rbp;
        tFrame.AddrFrame.Mode    = AddrModeFlat;
        tFrame.AddrStack.Offset  = _pContext->Rsp;
        tFrame.AddrStack.Mode    = AddrModeFlat;

        for (int i = 0; i < 32; ++i)
        {
            if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, hProcess, hThread, &tFrame, _pContext, nullptr
                           , SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
                break;
            if (0 == tFrame.AddrPC.Offset)
                break;

            char         arrBuffer[sizeof(SYMBOL_INFO) + 256] = {};
            SYMBOL_INFO* pSymbol  = reinterpret_cast<SYMBOL_INFO*>(arrBuffer);
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen   = 255;
            DWORD64 dwDisp        = 0;

            IMAGEHLP_LINE64 tLine = {};
            tLine.SizeOfStruct    = sizeof(tLine);
            DWORD dwLineDisp      = 0;

            const char* szFunc = SymFromAddr(hProcess, tFrame.AddrPC.Offset, &dwDisp, pSymbol) ? pSymbol->Name : "?";
            if (SymGetLineFromAddr64(hProcess, tFrame.AddrPC.Offset, &dwLineDisp, &tLine))
                CTestMgr::GetInst()->Log(L"  at %S (%S:%lu)", szFunc, tLine.FileName, tLine.LineNumber);
            else
                CTestMgr::GetInst()->Log(L"  at %S", szFunc);
        }
    }

    // 종료 감시: 테스트가 끝난 뒤에도 프로세스가 끝나지 않으면 메인 스레드 위치를 남기고 강제 종료한다
    DWORD g_dwMainThreadId = 0;

    DWORD WINAPI ExitWatchdog(LPVOID)
    {
        Sleep(10000);

        HANDLE hMain = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, g_dwMainThreadId);
        WriteCrtFile("\nHANG: process did not exit within 10s after the test finished\n");
        if (nullptr != hMain)
        {
            SuspendThread(hMain);
            CONTEXT tContext      = {};
            tContext.ContextFlags = CONTEXT_FULL;
            if (GetThreadContext(hMain, &tContext))
            {
                HANDLE hProcess = GetCurrentProcess();
                SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
                SymInitialize(hProcess, nullptr, TRUE);

                STACKFRAME64 tFrame     = {};
                tFrame.AddrPC.Offset    = tContext.Rip;
                tFrame.AddrPC.Mode      = AddrModeFlat;
                tFrame.AddrFrame.Offset = tContext.Rbp;
                tFrame.AddrFrame.Mode   = AddrModeFlat;
                tFrame.AddrStack.Offset = tContext.Rsp;
                tFrame.AddrStack.Mode   = AddrModeFlat;
                for (int i = 0; i < 40; ++i)
                {
                    if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, hProcess, hMain, &tFrame, &tContext, nullptr
                                   , SymFunctionTableAccess64, SymGetModuleBase64, nullptr) || 0 == tFrame.AddrPC.Offset)
                        break;

                    char         arrBuffer[sizeof(SYMBOL_INFO) + 256] = {};
                    SYMBOL_INFO* pSymbol  = reinterpret_cast<SYMBOL_INFO*>(arrBuffer);
                    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                    pSymbol->MaxNameLen   = 255;
                    DWORD64 dwDisp        = 0;
                    char    szLine[512]   = {};
                    sprintf_s(szLine, "  at %s\n", SymFromAddr(hProcess, tFrame.AddrPC.Offset, &dwDisp, pSymbol) ? pSymbol->Name : "?");
                    WriteCrtFile(szLine);
                }
            }
        }
        TerminateProcess(GetCurrentProcess(), 5);
        return 0;
    }

    LONG WINAPI CrashFilter(EXCEPTION_POINTERS* _pInfo)
    {
        CTestMgr::GetInst()->Log(L"CRASH code=0x%08X addr=%p", _pInfo->ExceptionRecord->ExceptionCode, _pInfo->ExceptionRecord->ExceptionAddress);
        WriteStackTrace(_pInfo->ContextRecord);
        CTestMgr::GetInst()->Abort(CTestMgr::EXIT_CRASH, L"unhandled exception");
        return EXCEPTION_EXECUTE_HANDLER;
    }

    void __cdecl InvalidParameterHandler(const wchar_t* _szExpr, const wchar_t* _szFunc, const wchar_t* _szFile, unsigned int _iLine, uintptr_t)
    {
        CTestMgr::GetInst()->Log(L"INVALID PARAMETER %s in %s (%s:%u)", _szExpr ? _szExpr : L"?", _szFunc ? _szFunc : L"?", _szFile ? _szFile : L"?", _iLine);
        CTestMgr::GetInst()->Abort(CTestMgr::EXIT_CRT_ERROR, L"invalid parameter");
    }

    void __cdecl PureCallHandler()
    {
        CTestMgr::GetInst()->Abort(CTestMgr::EXIT_CRASH, L"pure virtual call");
    }

    void __cdecl AbortHandler(int)
    {
        CTestMgr::GetInst()->Abort(CTestMgr::EXIT_CRT_ERROR, L"abort()");
    }
}

CTestMgr::CTestMgr()
    : m_bActive(false)
    , m_bFinished(false)
    , m_iExitCode(EXIT_PASS)
    , m_fFixedDT(1.f / 60.f)
    , m_iSeed(1)
    , m_iMaxFrame(600)
    , m_iFrame(0)
    , m_iLogEvery(1)
    , m_iMonLogEvery(0)
    , m_bNoSpawn(false)
    , m_bAllowLeaks(false)
    , m_bHeapCheck(false)
    , m_bDebugOnly(false)
    , m_bSkipped(false)
    , m_bHasStartLevel(false)
    , m_eStartLevel(LEVEL_TYPE::START)
    , m_iErrorCount(0)
    , m_iExpectCount(0)
    , m_iExpectFail(0)
    , m_pLog(nullptr)
{
}

CTestMgr::~CTestMgr()
{
    if (nullptr != m_pLog)
        fclose(m_pLog);
}

bool CTestMgr::Init(const wchar_t* _szCmdLine)
{
    if (nullptr == _szCmdLine || 0 == _szCmdLine[0])
        return false;

    int       iArgc   = 0;
    wchar_t** pArgv   = CommandLineToArgvW(_szCmdLine, &iArgc);
    if (nullptr == pArgv)
        return false;

    for (int i = 0; i < iArgc; ++i)
    {
        const wstring strArg = pArgv[i];
        if (L"--test" == strArg && i + 1 < iArgc)
            m_strScriptPath = pArgv[++i];
        else if (L"--out" == strArg && i + 1 < iArgc)
            m_strOutDir = pArgv[++i];
    }
    LocalFree(pArgv);

    if (m_strScriptPath.empty())
        return false;

    m_bActive = true;

    // 시나리오 이름 = 파일 이름(확장자 제외)
    size_t iSlash = m_strScriptPath.find_last_of(L"\\/");
    m_strName     = (wstring::npos == iSlash) ? m_strScriptPath : m_strScriptPath.substr(iSlash + 1);
    const size_t iDot = m_strName.find_last_of(L'.');
    if (wstring::npos != iDot)
        m_strName = m_strName.substr(0, iDot);

    if (m_strOutDir.empty())
        m_strOutDir = L".";

    // 출력 폴더를 절대 경로로 바꾸고 중간 폴더까지 만든다
    wchar_t szFull[MAX_PATH] = {};
    GetFullPathName(m_strOutDir.c_str(), MAX_PATH, szFull, nullptr);
    m_strOutDir = szFull;
    for (size_t i = 3; i <= m_strOutDir.size(); ++i)
    {
        if (i == m_strOutDir.size() || L'\\' == m_strOutDir[i] || L'/' == m_strOutDir[i])
            CreateDirectory(m_strOutDir.substr(0, i).c_str(), nullptr);
    }

    // 크래시·CRT 오류를 대화상자 대신 로그와 종료 코드로 받는다
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    SetUnhandledExceptionFilter(CrashFilter);
    _set_invalid_parameter_handler(InvalidParameterHandler);
    _set_purecall_handler(PureCallHandler);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    signal(SIGABRT, AbortHandler);

    g_hCrtFile = CreateFile(MakeOutPath(L".crt.txt").c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
#ifdef _DEBUG
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, CrtReportHook);
#endif

    _wfopen_s(&m_pLog, MakeOutPath(L".log").c_str(), L"w, ccs=UTF-8");

    if (!LoadScript())
    {
        m_iExitCode = EXIT_SCRIPT_ERROR;
        Finish();
        return true;
    }

#ifndef _DEBUG
    // 디버그 키처럼 Debug 빌드에만 있는 기능을 쓰는 시나리오는 건너뛴다
    if (m_bDebugOnly)
    {
        m_bSkipped = true;
        Finish();
        return true;
    }
#endif

#ifdef _DEBUG
    // 해제한 블록을 돌려주지 않고(이중 해제는 두 번째 free에서 바로 잡힌다) 1024번 할당마다 힙 전체를 검사한다(해제 후 쓰기)
    if (m_bHeapCheck)
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_EVERY_1024_DF);
#endif

    srand(m_iSeed);

#ifdef _DEBUG
    Log(L"SCRIPT %s build=Debug seed=%u dt=%f frames=%d", m_strScriptPath.c_str(), m_iSeed, m_fFixedDT, m_iMaxFrame);
#else
    Log(L"SCRIPT %s build=Release seed=%u dt=%f frames=%d", m_strScriptPath.c_str(), m_iSeed, m_fFixedDT, m_iMaxFrame);
#endif
    return true;
}

bool CTestMgr::LoadScript()
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, m_strScriptPath.c_str(), L"r, ccs=UTF-8");
    if (nullptr == pFile)
    {
        Log(L"SCRIPT ERROR cannot open %s", m_strScriptPath.c_str());
        return false;
    }

    vector<wstring> vecLine;
    wchar_t         szLine[1024] = {};
    while (fgetws(szLine, 1024, pFile))
    {
        wstring strLine = szLine;
        const size_t iHash = strLine.find(L'#');
        if (wstring::npos != iHash)
            strLine = strLine.substr(0, iHash);
        vecLine.push_back(strLine);
    }
    fclose(pFile);

    // 프레임 자리에 초("2.5s")를 쓸 수 있게 dt를 먼저 읽는다
    bool bOk = true;
    for (int iPass = 0; iPass < 2; ++iPass)
    {
        for (size_t i = 0; i < vecLine.size(); ++i)
        {
            const vector<wstring> vecTok = Split(vecLine[i]);
            const bool            bDT    = !vecTok.empty() && L"dt" == vecTok[0];
            if ((0 == iPass) != bDT)
                continue;
            if (!ParseLine(vecLine[i], static_cast<int>(i) + 1))
                bOk = false;
        }
    }
    return bOk;
}

bool CTestMgr::ParseLine(const wstring& _strLine, const int _iLine)
{
    const vector<wstring> vecTok = Split(_strLine);
    if (vecTok.empty())
        return true;

    const wstring& strCmd = vecTok[0];
    auto           Fail   = [&](const wchar_t* _szWhy)
    {
        Log(L"SCRIPT ERROR line %d: %s (%s)", _iLine, _szWhy, _strLine.c_str());
        return false;
    };

    int    iA = 0, iB = 0;
    double dA = 0.0;

    if (L"seed" == strCmd)
    {
        if (vecTok.size() != 2 || !ToInt(vecTok[1], iA)) return Fail(L"seed <n>");
        m_iSeed = static_cast<unsigned int>(iA);
    }
    else if (L"dt" == strCmd)
    {
        if (vecTok.size() != 2 || !ToDouble(vecTok[1], dA) || dA <= 0.0) return Fail(L"dt <seconds>");
        m_fFixedDT = static_cast<float>(dA);
    }
    else if (L"frames" == strCmd)
    {
        if (vecTok.size() != 2 || !ParseFrame(vecTok[1], iA) || iA <= 0) return Fail(L"frames <n|seconds s>");
        m_iMaxFrame = iA;
    }
    else if (L"start" == strCmd)
    {
        if (vecTok.size() != 2 || !StrToLevel(vecTok[1].c_str(), m_eStartLevel)) return Fail(L"start <LEVEL>");
        m_bHasStartLevel = true;
    }
    else if (L"log" == strCmd)
    {
        if (vecTok.size() != 2 || !ToInt(vecTok[1], iA) || iA < 0) return Fail(L"log <every>");
        m_iLogEvery = iA;
    }
    else if (L"logmon" == strCmd)
    {
        if (vecTok.size() != 2 || !ToInt(vecTok[1], iA) || iA < 0) return Fail(L"logmon <every>");
        m_iMonLogEvery = iA;
    }
    else if (L"nospawn" == strCmd)
    {
        m_bNoSpawn = true;
    }
    else if (L"allowleaks" == strCmd)
    {
        m_bAllowLeaks = true;
    }
    else if (L"heapcheck" == strCmd)
    {
        m_bHeapCheck = true;
    }
    else if (L"debugonly" == strCmd)
    {
        m_bDebugOnly = true;
    }
    else if (L"hold" == strCmd || L"tap" == strCmd)
    {
        const bool   bHold  = L"hold" == strCmd;
        const size_t iFirst = bHold ? 3 : 2;
        if (vecTok.size() <= iFirst || !ParseFrame(vecTok[1], iA)) return Fail(bHold ? L"hold <from> <to> <KEY...>" : L"tap <frame> <KEY...>");
        if (bHold && !ParseFrame(vecTok[2], iB)) return Fail(L"hold <from> <to> <KEY...>");
        if (!bHold) iB = iA;

        for (size_t i = iFirst; i < vecTok.size(); ++i)
        {
            UINT iKey = 0;
            if (!StrToKey(vecTok[i], iKey)) return Fail(L"unknown key");
            m_vecKey.push_back(tKeyRange{ iA, iB, iKey });
        }
    }
    else if (L"mouse" == strCmd)
    {
        double dX = 0.0, dY = 0.0;
        if (vecTok.size() != 4 || !ParseFrame(vecTok[1], iA) || !ToDouble(vecTok[2], dX) || !ToDouble(vecTok[3], dY)) return Fail(L"mouse <frame> <x> <y>");
        m_vecMouse.push_back(tMouse{ iA, Vec2(static_cast<float>(dX), static_cast<float>(dY)) });
    }
    else if (L"dialog" == strCmd)
    {
        if (vecTok.size() != 2) return Fail(L"dialog <path>");
        m_queDialogPath.push_back(vecTok[1]);
    }
    else if (L"spike" == strCmd)
    {
        if (vecTok.size() != 3 || !ParseFrame(vecTok[1], iA) || !ToDouble(vecTok[2], dA) || dA <= 0.0) return Fail(L"spike <frame> <dt>");
        m_mapSpikeDT[iA] = static_cast<float>(dA);
    }
    else if (L"watch" == strCmd || L"watchanim" == strCmd || L"trace" == strCmd)
    {
        if (vecTok.size() != 2) return Fail(L"watch|watchanim|trace <Class>");
        if (L"watch" == strCmd)
            m_vecWatch.push_back(vecTok[1]);
        else if (L"watchanim" == strCmd)
            m_vecWatchAnim.push_back(vecTok[1]);
        else
            m_vecTrace.push_back(vecTok[1]);
    }
    else if (L"level" == strCmd || L"spawn" == strCmd || L"god" == strCmd || L"hmg" == strCmd
          || L"capture" == strCmd || L"expect" == strCmd || L"teleport" == strCmd || L"timer" == strCmd
          || L"kill" == strCmd || L"play" == strCmd || L"loadanim" == strCmd || L"loadtex" == strCmd)
    {
        if (vecTok.size() < (L"kill" == strCmd ? 2u : 3u)) return Fail(L"<command> <frame|end> ...");

        tCommand tCmd = {};
        tCmd.strCmd   = strCmd;
        tCmd.iLine    = _iLine;
        if (L"end" == vecTok[1])
            tCmd.iFrame = -1;
        else if (!ParseFrame(vecTok[1], tCmd.iFrame) || tCmd.iFrame < 0)
            return Fail(L"frame must be a number, seconds ('2.5s') or 'end'");

        if (vecTok.size() > 2) tCmd.strArg1 = vecTok[2];
        if (vecTok.size() > 3) tCmd.strArg2 = vecTok[3];
        if (vecTok.size() > 4) tCmd.strArg3 = vecTok[4];
        if (vecTok.size() > 5) tCmd.strArg4 = vecTok[5];

        LEVEL_TYPE eLevel  = LEVEL_TYPE::START;
        MONSTER    eType   = MONSTER::SOLDIER;
        double     dX = 0.0, dY = 0.0;
        if (L"level" == strCmd && !StrToLevel(tCmd.strArg1.c_str(), eLevel)) return Fail(L"level <frame> <LEVEL>");
        if (L"spawn" == strCmd && (!StrToMonster(tCmd.strArg1, eType) || !ToDouble(tCmd.strArg2, dX) || !ToDouble(tCmd.strArg3, dY)))
            return Fail(L"spawn <frame> <TYPE> <x> <y>");
        if ((L"god" == strCmd || L"hmg" == strCmd) && L"0" != tCmd.strArg1 && L"1" != tCmd.strArg1) return Fail(L"god|hmg <frame> <0|1>");
        if (L"teleport" == strCmd && (!ToDouble(tCmd.strArg1, dX) || !ToDouble(tCmd.strArg2, dY))) return Fail(L"teleport <frame> <x> <y>");
        if (L"timer" == strCmd && !ToInt(tCmd.strArg1, iA)) return Fail(L"timer <frame> <n>");
        if ((L"play" == strCmd || L"loadanim" == strCmd || L"loadtex" == strCmd) && tCmd.strArg2.empty())
            return Fail(L"play|loadanim <frame> <Class> <name> / loadtex <frame> <key> <path>");
        if (L"expect" == strCmd && (tCmd.strArg2.empty() || tCmd.strArg3.empty())) return Fail(L"expect <frame|end> <subject> <op> <value>");

        m_vecCommand.push_back(tCmd);
    }
    else
    {
        return Fail(L"unknown command");
    }

    return true;
}

bool CTestMgr::ParseFrame(const wstring& _str, int& _iOut) const
{
    // "2.5s" 는 초. 고정 DT로 프레임 수를 구한다 (DT가 다른 시나리오끼리 같은 시각을 쓰기 위해)
    if (_str.size() > 1 && L's' == _str.back())
    {
        double dSec = 0.0;
        if (!ToDouble(_str.substr(0, _str.size() - 1), dSec) || dSec < 0.0)
            return false;
        _iOut = static_cast<int>(std::lround(dSec / static_cast<double>(m_fFixedDT)));
        return true;
    }
    return ToInt(_str, _iOut);
}

float CTestMgr::GetFixedDT() const
{
    const auto iter = m_mapSpikeDT.find(m_iFrame);
    return iter == m_mapSpikeDT.end() ? m_fFixedDT : iter->second;
}

void CTestMgr::AddFrameTiming(const double _dTickMs, const double _dRenderMs)
{
    // 시작 레벨 진입 직후 몇 프레임은 리소스를 처음 읽느라 느리므로 뺀다
    constexpr int WARMUP_FRAMES = 30;
    if (!m_bActive || m_iFrame < WARMUP_FRAMES)
        return;
    m_dTickMsSum   += _dTickMs;
    m_dRenderMsSum += _dRenderMs;
    ++m_iTimedFrames;
}

void CTestMgr::OnAnimParse(const wstring& _strPath)
{
    if (!m_bActive)
        return;
    ++m_mapCounter[L"anim.parse"];

    // 같은 파일을 "Enemy//a.anim"·"Enemy\\a.anim"처럼 다르게 적어도 하나로 센다
    wstring strKey;
    for (const wchar_t ch : _strPath)
    {
        const wchar_t c = (L'/' == ch) ? L'\\' : static_cast<wchar_t>(towlower(ch));
        if (L'\\' == c && !strKey.empty() && L'\\' == strKey.back())
            continue;
        strKey += c;
    }
    m_setAnimFile.insert(strKey);
}

void CTestMgr::AddCount(const wchar_t* _szName, const long long _llAdd)
{
    if (m_bActive)
        m_mapCounter[_szName] += _llAdd;
}

bool CTestMgr::IsKeyDown(const KEY _eKey) const
{
    const UINT iKey = static_cast<UINT>(_eKey);
    for (const tKeyRange& tRange : m_vecKey)
    {
        if (tRange.iKey == iKey && tRange.iFrom <= m_iFrame && m_iFrame <= tRange.iTo)
            return true;
    }
    return false;
}

Vec2 CTestMgr::GetMousePos() const
{
    Vec2 vPos = {};
    int  iBest = -1;
    for (const tMouse& tMs : m_vecMouse)
    {
        if (tMs.iFrame <= m_iFrame && tMs.iFrame > iBest)
        {
            iBest = tMs.iFrame;
            vPos  = tMs.vPos;
        }
    }
    return vPos;
}

bool CTestMgr::PopDialogPath(wstring& _strPath)
{
    if (m_queDialogPath.empty())
        return false;
    _strPath = m_queDialogPath.front();
    m_queDialogPath.pop_front();
    return true;
}

void CTestMgr::OnFrameEnd()
{
    if (!m_bActive || m_bFinished)
        return;

    TrackWatches();

    for (const tCommand& tCmd : m_vecCommand)
    {
        if (tCmd.iFrame == m_iFrame)
            RunCommand(tCmd);
    }

    if (m_iLogEvery > 0 && 0 == m_iFrame % m_iLogEvery)
        LogState();
    if (m_iMonLogEvery > 0 && 0 == m_iFrame % m_iMonLogEvery)
        LogMonsters();

    if (m_iFrame + 1 >= m_iMaxFrame)
    {
        for (const tCommand& tCmd : m_vecCommand)
        {
            if (-1 == tCmd.iFrame)
                RunCommand(tCmd);
        }
        Finish();
        return;
    }

    ++m_iFrame;
}

void CTestMgr::RunCommand(const tCommand& _tCmd)
{
    if (L"expect" == _tCmd.strCmd)
    {
        CheckExpect(_tCmd);
    }
    else if (L"level" == _tCmd.strCmd)
    {
        LEVEL_TYPE eLevel = LEVEL_TYPE::START;
        StrToLevel(_tCmd.strArg1.c_str(), eLevel);
        Log(L"CMD %d level %s", m_iFrame, _tCmd.strArg1.c_str());
        ChangeLevel(eLevel);
    }
    else if (L"capture" == _tCmd.strCmd)
    {
        Capture(_tCmd.strArg1);
    }
    else if (L"god" == _tCmd.strCmd || L"hmg" == _tCmd.strCmd)
    {
        CPlayer* pPlayer = FindPlayer();
        if (nullptr == pPlayer)
        {
            Log(L"CMD %d %s: no player (line %d)", m_iFrame, _tCmd.strCmd.c_str(), _tCmd.iLine);
            return;
        }
        const bool bOn = L"1" == _tCmd.strArg1;
        if (L"god" == _tCmd.strCmd)
            pPlayer->SetGodMode(bOn);
        else
            pPlayer->SetHMGMode(bOn);
        Log(L"CMD %d %s %d", m_iFrame, _tCmd.strCmd.c_str(), bOn ? 1 : 0);
    }
    else if (L"kill" == _tCmd.strCmd)
    {
        CPlayer* pPlayer = FindPlayer();
        if (nullptr == pPlayer)
        {
            Log(L"CMD %d kill: no player (line %d)", m_iFrame, _tCmd.iLine);
            return;
        }
        pPlayer->Kill();
        Log(L"CMD %d kill", m_iFrame);
    }
    else if (L"teleport" == _tCmd.strCmd)
    {
        CPlayer* pPlayer = FindPlayer();
        double   dX = 0.0, dY = 0.0;
        ToDouble(_tCmd.strArg1, dX);
        ToDouble(_tCmd.strArg2, dY);
        if (nullptr == pPlayer)
        {
            Log(L"CMD %d teleport: no player (line %d)", m_iFrame, _tCmd.iLine);
            return;
        }
        pPlayer->SetPos(CCamera::GetInst()->GetRealPos(Vec2(static_cast<float>(dX), static_cast<float>(dY))));   // 화면 좌표 -> 월드
        Log(L"CMD %d teleport (%.1f,%.1f)", m_iFrame, dX, dY);
    }
    else if (L"play" == _tCmd.strCmd || L"loadanim" == _tCmd.strCmd)
    {
        // 그 클래스의 첫 오브젝트 애니메이터에서 재생·불러오기 (리소스가 없을 때 어떻게 되는지 보는 용도)
        CObj*   pTarget = nullptr;
        CLevel* pLevel  = CLevelMgr::GetInst()->GetCurLevel();
        for (UINT i = 0; nullptr == pTarget && nullptr != pLevel && i < static_cast<UINT>(LAYER::END); ++i)
            for (CObj* pObj : pLevel->GetLayer(static_cast<LAYER>(i)))
                if (!pObj->IsDead() && ClassName(pObj) == _tCmd.strArg1 && nullptr != pObj->GetAnimator())
                {
                    pTarget = pObj;
                    break;
                }
        if (nullptr == pTarget)
        {
            Log(L"CMD %d %s: no %s (line %d)", m_iFrame, _tCmd.strCmd.c_str(), _tCmd.strArg1.c_str(), _tCmd.iLine);
            return;
        }
        Log(L"CMD %d %s %s %s", m_iFrame, _tCmd.strCmd.c_str(), _tCmd.strArg1.c_str(), _tCmd.strArg2.c_str());
        if (L"loadanim" == _tCmd.strCmd)
        {
            pTarget->GetAnimator()->LoadAnimation(_tCmd.strArg2);
            return;
        }
        // 게임 코드처럼 재생한 뒤 바로 방향을 정한다
        pTarget->GetAnimator()->Play(_tCmd.strArg2, false);
        pTarget->GetAnimator()->GetAnimation()->SetPlayRight();
    }
    else if (L"loadtex" == _tCmd.strCmd)
    {
        const CTexture* pTex = CResMgr::GetInst()->LoadTexture(_tCmd.strArg1, _tCmd.strArg2);
        Log(L"CMD %d loadtex %s %s -> %ux%u", m_iFrame, _tCmd.strArg1.c_str(), _tCmd.strArg2.c_str()
          , nullptr == pTex ? 0u : pTex->Width(), nullptr == pTex ? 0u : pTex->Height());
    }
    else if (L"timer" == _tCmd.strCmd)
    {
        CStage* pStage = dynamic_cast<CStage*>(CLevelMgr::GetInst()->GetCurLevel());
        int     iTime  = 0;
        ToInt(_tCmd.strArg1, iTime);
        if (nullptr == pStage)
        {
            Log(L"CMD %d timer: not in STAGE (line %d)", m_iFrame, _tCmd.iLine);
            return;
        }
        pStage->SetRemainTime(iTime);
        Log(L"CMD %d timer %d", m_iFrame, iTime);
    }
    else if (L"spawn" == _tCmd.strCmd)
    {
        CStage* pStage = dynamic_cast<CStage*>(CLevelMgr::GetInst()->GetCurLevel());
        MONSTER eType  = MONSTER::SOLDIER;
        double  dX = 0.0, dY = 0.0;
        StrToMonster(_tCmd.strArg1, eType);
        ToDouble(_tCmd.strArg2, dX);
        ToDouble(_tCmd.strArg3, dY);
        if (nullptr == pStage)
        {
            Log(L"CMD %d spawn: not in STAGE (line %d)", m_iFrame, _tCmd.iLine);
            return;
        }
        pStage->SpawnMonsterAt(eType, Vec2(static_cast<float>(dX), static_cast<float>(dY)));
        Log(L"CMD %d spawn %s (%.1f,%.1f)", m_iFrame, _tCmd.strArg1.c_str(), dX, dY);
    }
}

bool CTestMgr::GetSubject(const wstring& _strSubject, wstring& _strValue, double& _dValue, bool& _bNumber)
{
    _bNumber = true;
    _dValue  = 0.0;
    _strValue.clear();

    auto CountOf = [](const map<wstring, int>& _map, const wstring& _strKey)
    {
        const auto iter = _map.find(_strKey);
        return iter == _map.end() ? 0 : iter->second;
    };

    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

    if (L"frame" == _strSubject)            { _dValue = m_iFrame; return true; }
    if (L"errors" == _strSubject)           { _dValue = m_iErrorCount; return true; }
    if (L"gdi" == _strSubject)              { _dValue = static_cast<double>(GdiCount()); return true; }
    if (L"mem" == _strSubject)              { _dValue = static_cast<double>(PrivateBytes()); return true; }
    if (L"level" == _strSubject)
    {
        _bNumber  = false;
        _strValue = LevelToStr(CLevelMgr::GetInst()->GetCurLevelType());
        return true;
    }
    if (0 == _strSubject.compare(0, 8, L"created."))  { _dValue = CountOf(m_mapCreated, _strSubject.substr(8)); return true; }
    if (0 == _strSubject.compare(0, 8, L"deleted."))  { _dValue = CountOf(m_mapDeleted, _strSubject.substr(8)); return true; }
    if (0 == _strSubject.compare(0, 6, L"enter."))    { _dValue = CountOf(m_mapLevelEnter, _strSubject.substr(6)); return true; }
    if (0 == _strSubject.compare(0, 6, L"alive."))
    {
        const wstring strClass = _strSubject.substr(6);
        int           iCount   = 0;
        if (nullptr != pLevel)
        {
            for (UINT i = 0; i < static_cast<UINT>(LAYER::END); ++i)
            {
                for (CObj* pObj : pLevel->GetLayer(static_cast<LAYER>(i)))
                {
                    if (!pObj->IsDead() && ClassName(pObj) == strClass)
                        ++iCount;
                }
            }
        }
        _dValue = iCount;
        return true;
    }
    if (0 == _strSubject.compare(0, 6, L"layer."))
    {
        // layer.<번호> : 그 레이어의 살아 있는 오브젝트 수
        int iLayer = 0;
        if (!ToInt(_strSubject.substr(6), iLayer) || iLayer < 0 || iLayer >= static_cast<int>(LAYER::END))
            return false;
        int iCount = 0;
        if (nullptr != pLevel)
        {
            for (CObj* pObj : pLevel->GetLayer(static_cast<LAYER>(iLayer)))
                if (!pObj->IsDead())
                    ++iCount;
        }
        _dValue = iCount;
        return true;
    }
    if (0 == _strSubject.compare(0, 5, L"stat."))
    {
        const auto iter = m_mapStat.find(_strSubject.substr(5));
        if (iter == m_mapStat.end())
            return false;
        _dValue = static_cast<double>(iter->second);
        return true;
    }
    if (0 == _strSubject.compare(0, 7, L"player."))
    {
        CPlayer* pPlayer = FindPlayer();
        if (nullptr == pPlayer)
            return false;

        const wstring strField = _strSubject.substr(7);
        if (L"life" == strField)   { _dValue = pPlayer->GetLifeCount(); return true; }
        if (L"bomb" == strField)   { _dValue = pPlayer->GetBombCount(); return true; }
        if (L"bullet" == strField) { _dValue = pPlayer->GetBulletCount(); return true; }
        if (L"hmg" == strField)    { _dValue = pPlayer->IsHMG() ? 1 : 0; return true; }
        if (L"x" == strField)      { _dValue = ScreenPos(pPlayer).x; return true; }
        if (L"y" == strField)      { _dValue = ScreenPos(pPlayer).y; return true; }
        if (L"anim" == strField)   { _bNumber = false; _strValue = AnimName(pPlayer->GetAnimator()); return true; }
        if (L"anim2" == strField)  { _bNumber = false; _strValue = AnimName(pPlayer->GetAnimator2()); return true; }
        return false;
    }
    if (L"monsters" == _strSubject)
    {
        int iCount = 0;
        if (nullptr != pLevel)
            for (CObj* pObj : pLevel->GetLayer(LAYER::MONSTER))
                if (!pObj->IsDead())
                    ++iCount;
        _dValue = iCount;
        return true;
    }
    if (L"cam.x" == _strSubject || L"cam.y" == _strSubject)
    {
        const Vec2 vLT = CameraLT();
        _dValue        = (L"cam.x" == _strSubject) ? vLT.x : vLT.y;
        return true;
    }
    if (L"perf.tick" == _strSubject || L"perf.render" == _strSubject)
    {
        // 프레임 평균 시간(ms). 빌드 구성·PC에 따라 다르니 같은 조건에서 전후를 비교할 때만 쓴다
        const double dSum = L"perf.tick" == _strSubject ? m_dTickMsSum : m_dRenderMsSum;
        _dValue           = m_iTimedFrames > 0 ? dSum / m_iTimedFrames : 0.0;
        return true;
    }
    if (L"anim.files" == _strSubject)   { _dValue = static_cast<double>(m_setAnimFile.size()); return true; }
    if (L"overlaps" == _strSubject)     { _dValue = static_cast<double>(CCollisionMgr::GetInst()->GetOverlapCount()); return true; }
    if (L"cam.fade" == _strSubject)     { _dValue = CCamera::GetInst()->GetFadeRatio(); return true; }
    if (L"cam.offset.x" == _strSubject) { _dValue = CCamera::GetInst()->GetOffset().x; return true; }
    if (L"stage.time" == _strSubject)
    {
        CStage* pStage = dynamic_cast<CStage*>(pLevel);
        if (nullptr == pStage)
            return false;
        _dValue = pStage->GetRemainTime();
        return true;
    }
    if (0 == _strSubject.compare(0, 6, L"count."))
    {
        const auto iter = m_mapCounter.find(_strSubject.substr(6));
        _dValue         = iter == m_mapCounter.end() ? 0.0 : static_cast<double>(iter->second);
        return true;
    }
    // firstframe.created.<클래스> · firsttime.deleted.<클래스> · firsttime.anim.<클래스>.<애니메이션> 등
    // : 처음 생성·삭제된, 또는 그 애니메이션을 처음 시작한(watchanim) 프레임·시각(초). 없으면 -1
    if (0 == _strSubject.compare(0, 11, L"firstframe.") || 0 == _strSubject.compare(0, 10, L"firsttime."))
    {
        const bool               bTime    = L't' == _strSubject[5];
        const wstring            strRest  = _strSubject.substr(bTime ? 10 : 11);
        const map<wstring, int>* pMap     = nullptr;
        size_t                   iKeyFrom = 0;
        if (0 == strRest.compare(0, 8, L"created."))      { pMap = &m_mapFirstCreated; iKeyFrom = 8; }
        else if (0 == strRest.compare(0, 8, L"deleted.")) { pMap = &m_mapFirstDeleted; iKeyFrom = 8; }
        else if (0 == strRest.compare(0, 5, L"anim."))    { pMap = &m_mapFirstAnim;    iKeyFrom = 5; }
        else
            return false;
        const auto iter = pMap->find(strRest.substr(iKeyFrom));
        if (iter == pMap->end())
        {
            _dValue = -1.0;
            return true;
        }
        _dValue = bTime ? iter->second * static_cast<double>(m_fFixedDT) : iter->second;
        return true;
    }
    // delfrm.<클래스> : 그 클래스에서 처음 삭제된 오브젝트가 지워질 때 보이던 애니메이션 프레임 번호. 없으면 -1
    if (0 == _strSubject.compare(0, 7, L"delfrm."))
    {
        const auto iter = m_mapDeletedFrm.find(_strSubject.substr(7));
        _dValue         = iter == m_mapDeletedFrm.end() ? -1.0 : iter->second;
        return true;
    }
    // lifetime.<클래스> : 그 클래스에서 처음 삭제된 오브젝트가 생성부터 삭제까지 산 시간(초). 없으면 -1
    if (0 == _strSubject.compare(0, 9, L"lifetime."))
    {
        const auto iter = m_mapLifetime.find(_strSubject.substr(9));
        _dValue         = iter == m_mapLifetime.end() ? -1.0 : iter->second * static_cast<double>(m_fFixedDT);
        return true;
    }
    if (0 == _strSubject.compare(0, 10, L"animcount."))
    {
        const auto iter = m_mapAnimCount.find(_strSubject.substr(10));
        _dValue         = iter == m_mapAnimCount.end() ? 0.0 : iter->second;
        return true;
    }
    // maxcollag.<클래스> : watch 로 본 충돌체 지연 거리의 최댓값 (0이면 늘 이번 프레임 위치로 판정)
    if (0 == _strSubject.compare(0, 10, L"maxcollag."))
    {
        const auto iter = m_mapMaxColLag.find(_strSubject.substr(10));
        _dValue         = iter == m_mapMaxColLag.end() ? 0.0 : iter->second;
        return true;
    }
    // wrange.<클래스>.x : watch 로 추적한 월드 위치의 폭(최댓값 - 최솟값). 가만히 있는 오브젝트가 배경에 붙어 있으면 0
    if (0 == _strSubject.compare(0, 7, L"wrange."))
    {
        const size_t iDot = _strSubject.find_last_of(L'.');
        if (iDot <= 7)
            return false;
        const wstring strClass = _strSubject.substr(7, iDot - 7);
        const bool    bX       = L"x" == _strSubject.substr(iDot + 1);
        const auto    iterMin  = m_mapWMin.find(strClass);
        if (iterMin == m_mapWMin.end())
        {
            _dValue = std::numeric_limits<double>::quiet_NaN();
            return true;
        }
        const Vec2 vMax = m_mapWMax[strClass];
        _dValue         = bX ? vMax.x - iterMin->second.x : vMax.y - iterMin->second.y;
        return true;
    }
    // min.<클래스>.x / max.<클래스>.y : watch 로 추적한 위치 범위
    if (0 == _strSubject.compare(0, 4, L"min.") || 0 == _strSubject.compare(0, 4, L"max."))
    {
        const size_t iDot = _strSubject.find_last_of(L'.');
        if (iDot <= 4)
            return false;
        const wstring strClass = _strSubject.substr(4, iDot - 4);
        const wstring strAxis  = _strSubject.substr(iDot + 1);
        const auto&   mapRange = (L'm' == _strSubject[0] && L'i' == _strSubject[1]) ? m_mapMin : m_mapMax;
        const auto    iter     = mapRange.find(strClass);
        if (iter == mapRange.end())
        {
            _dValue = std::numeric_limits<double>::quiet_NaN();
            return true;
        }
        _dValue = (L"x" == strAxis) ? iter->second.x : iter->second.y;
        return true;
    }
    // first.<클래스>.<필드> : 살아 있는 첫 오브젝트, last.<클래스>.<필드> : 가장 최근에 만든 오브젝트
    if (0 == _strSubject.compare(0, 6, L"first.") || 0 == _strSubject.compare(0, 5, L"last."))
    {
        const bool   bFirst = L'f' == _strSubject[0];
        const size_t iStart = bFirst ? 6 : 5;
        const size_t iDot   = _strSubject.find(L'.', iStart);
        if (wstring::npos == iDot || nullptr == pLevel)
            return false;
        const wstring strClass = _strSubject.substr(iStart, iDot - iStart);
        const wstring strField = _strSubject.substr(iDot + 1);

        CObj* pFound = nullptr;
        for (UINT i = 0; i < static_cast<UINT>(LAYER::END); ++i)
        {
            for (CObj* pObj : pLevel->GetLayer(static_cast<LAYER>(i)))
            {
                if (pObj->IsDead() || ClassName(pObj) != strClass)
                    continue;
                if (nullptr == pFound || (!bFirst && pObj->GetID() > pFound->GetID()))
                    pFound = pObj;
            }
        }
        if (nullptr == pFound)
        {
            _bNumber  = false;
            _strValue = L"none";
            return true;
        }
        return GetObjectField(pFound, strField, _strValue, _dValue, _bNumber);
    }

    return false;
}

bool CTestMgr::GetObjectField(CObj* _pObj, const wstring& _strField, wstring& _strValue, double& _dValue, bool& _bNumber)
{
    _bNumber = true;
    if (L"x" == _strField)    { _dValue = ScreenPos(_pObj).x; return true; }
    if (L"y" == _strField)    { _dValue = ScreenPos(_pObj).y; return true; }
    if (L"wx" == _strField)   { _dValue = WorldPos(_pObj).x; return true; }
    if (L"wy" == _strField)   { _dValue = WorldPos(_pObj).y; return true; }
    if (L"id" == _strField)   { _dValue = _pObj->GetID(); return true; }
    if (L"dead" == _strField) { _dValue = _pObj->IsDead() ? 1 : 0; return true; }
    if (L"collag" == _strField)
    {
        _dValue = ColliderLag(_pObj);
        return _dValue >= 0.0;
    }
    if (L"colw" == _strField || L"colh" == _strField)
    {
        if (nullptr == _pObj->GetCollider())
            return false;
        _dValue = (L"colw" == _strField) ? _pObj->GetCollider()->GetScale().x : _pObj->GetCollider()->GetScale().y;
        return true;
    }
    if (L"hp" == _strField)
    {
        CMonster* pMon = dynamic_cast<CMonster*>(_pObj);
        if (nullptr == pMon)
            return false;
        _dValue = pMon->GetHP();
        return true;
    }
    if (L"anim" == _strField)  { _bNumber = false; _strValue = AnimName(_pObj->GetAnimator()); return true; }
    if (L"anim2" == _strField) { _bNumber = false; _strValue = AnimName(_pObj->GetAnimator2()); return true; }
    if (L"frm" == _strField)   { _dValue = AnimFrame(_pObj->GetAnimator()); return true; }
    if (L"frm2" == _strField)  { _dValue = AnimFrame(_pObj->GetAnimator2()); return true; }
    return false;
}

void CTestMgr::TrackWatches()
{
    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
    if (nullptr == pLevel || (m_vecWatch.empty() && m_vecWatchAnim.empty() && m_vecTrace.empty()))
        return;

    for (UINT i = 0; i < static_cast<UINT>(LAYER::END); ++i)
    {
        for (CObj* pObj : pLevel->GetLayer(static_cast<LAYER>(i)))
        {
            if (pObj->IsDead())
                continue;
            const wstring strClass = ClassName(pObj);

            if (std::find(m_vecWatch.begin(), m_vecWatch.end(), strClass) != m_vecWatch.end())
            {
                // 이번 프레임 끝에 막 생긴 오브젝트는 아직 tick 전이라 충돌체 위치가 없다. 그다음 프레임부터 본다
                const auto iterCreate = m_mapCreateFrame.find(pObj->GetID());
                if (iterCreate == m_mapCreateFrame.end() || iterCreate->second < m_iFrame)
                {
                    const double dLag = ColliderLag(pObj);
                    if (dLag > m_mapMaxColLag[strClass])
                        m_mapMaxColLag[strClass] = dLag;
                }

                const Vec2 vWorld = WorldPos(pObj);
                auto       iterW  = m_mapWMin.find(strClass);
                if (iterW == m_mapWMin.end())
                {
                    m_mapWMin[strClass] = vWorld;
                    m_mapWMax[strClass] = vWorld;
                }
                else
                {
                    Vec2& vMin = iterW->second;
                    Vec2& vMax = m_mapWMax[strClass];
                    vMin       = Vec2(min(vMin.x, vWorld.x), min(vMin.y, vWorld.y));
                    vMax       = Vec2(max(vMax.x, vWorld.x), max(vMax.y, vWorld.y));
                }

                const Vec2 vPos = ScreenPos(pObj);
                auto       iter = m_mapMin.find(strClass);
                if (iter == m_mapMin.end())
                {
                    m_mapMin[strClass] = vPos;
                    m_mapMax[strClass] = vPos;
                }
                else
                {
                    Vec2& vMin = iter->second;
                    Vec2& vMax = m_mapMax[strClass];
                    vMin       = Vec2(min(vMin.x, vPos.x), min(vMin.y, vPos.y));
                    vMax       = Vec2(max(vMax.x, vPos.x), max(vMax.y, vPos.y));
                }
            }

            if (std::find(m_vecTrace.begin(), m_vecTrace.end(), strClass) != m_vecTrace.end())
            {
                const Vec2 vPos = ScreenPos(pObj);
                Log(L"T %d %s id=%u (%.2f,%.2f) a=%s#%d a2=%s#%d", m_iFrame, strClass.c_str(), pObj->GetID(), vPos.x, vPos.y
                  , AnimName(pObj->GetAnimator()), AnimFrame(pObj->GetAnimator()), AnimName(pObj->GetAnimator2()), AnimFrame(pObj->GetAnimator2()));
            }

            if (std::find(m_vecWatchAnim.begin(), m_vecWatchAnim.end(), strClass) != m_vecWatchAnim.end())
            {
                const wstring strAnim = AnimName(pObj->GetAnimator());
                wstring&      strLast = m_mapLastAnim[pObj->GetID()];
                if (strLast != strAnim)
                {
                    ++m_mapAnimCount[strClass + L"." + strAnim];
                    Log(L"A %d %s id=%u %s -> %s", m_iFrame, strClass.c_str(), pObj->GetID(), strLast.empty() ? L"-" : strLast.c_str(), strAnim.c_str());
                    strLast = strAnim;
                }

                // 두 애니메이터의 애니메이션·프레임을 처음 보인 프레임 (firstframe.anim.<클래스>.<애니메이션>[@<프레임 번호>]. #은 시나리오 주석이라 @)
                for (const CAnimator* pAnimator : { pObj->GetAnimator(), pObj->GetAnimator2() })
                {
                    if (nullptr == pAnimator || nullptr == pAnimator->GetAnimation())
                        continue;
                    const wstring strKey = strClass + L"." + AnimName(pAnimator);
                    m_mapFirstAnim.emplace(strKey, m_iFrame);
                    m_mapFirstAnim.emplace(strKey + L"@" + std::to_wstring(AnimFrame(pAnimator)), m_iFrame);
                }
            }
        }
    }
}

void CTestMgr::CheckExpect(const tCommand& _tCmd)
{
    ++m_iExpectCount;

    wstring strLeft;
    double  dLeft   = 0.0;
    bool    bNumber = true;
    if (!GetSubject(_tCmd.strArg1, strLeft, dLeft, bNumber))
    {
        ++m_iExpectFail;
        Log(L"X %d FAIL line %d: unknown subject '%s'", m_iFrame, _tCmd.iLine, _tCmd.strArg1.c_str());
        return;
    }

    // 오른쪽 값은 숫자, 문자열, 또는 다른 subject
    wstring strRight = _tCmd.strArg3;
    double  dRight   = 0.0;
    bool    bRightOk = bNumber ? ToDouble(strRight, dRight) : true;
    if (bNumber && !bRightOk)
    {
        // <대상>, <대상>+<숫자>, <대상>-<숫자>
        wstring      strSubject = strRight;
        double       dAdd       = 0.0;
        const size_t iSign      = strRight.find_last_of(L"+-");
        if (wstring::npos != iSign && 0 < iSign && ToDouble(strRight.substr(iSign + 1), dAdd))
        {
            if (L'-' == strRight[iSign])
                dAdd = -dAdd;
            strSubject = strRight.substr(0, iSign);
        }

        wstring strTmp;
        bool    bRightNumber = true;
        bRightOk             = GetSubject(strSubject, strTmp, dRight, bRightNumber) && bRightNumber;
        dRight              += dAdd;
    }
    if (!bRightOk)
    {
        ++m_iExpectFail;
        Log(L"X %d FAIL line %d: bad right value '%s'", m_iFrame, _tCmd.iLine, strRight.c_str());
        return;
    }

    const wstring& strOp = _tCmd.strArg2;
    bool           bPass = false;
    if (bNumber)
    {
        constexpr double EPS = 1e-4;
        if (L"==" == strOp)      bPass = fabs(dLeft - dRight) <= EPS;
        else if (L"!=" == strOp) bPass = fabs(dLeft - dRight) > EPS;
        else if (L"<" == strOp)  bPass = dLeft < dRight;
        else if (L"<=" == strOp) bPass = dLeft <= dRight + EPS;
        else if (L">" == strOp)  bPass = dLeft > dRight;
        else if (L">=" == strOp) bPass = dLeft >= dRight - EPS;
        Log(L"X %d %s line %d: %s %s %s (actual %g, right %g)", m_iFrame, bPass ? L"PASS" : L"FAIL", _tCmd.iLine
          , _tCmd.strArg1.c_str(), strOp.c_str(), strRight.c_str(), dLeft, dRight);
    }
    else
    {
        if (L"==" == strOp)      bPass = strLeft == strRight;
        else if (L"!=" == strOp) bPass = strLeft != strRight;
        Log(L"X %d %s line %d: %s %s %s (actual %s)", m_iFrame, bPass ? L"PASS" : L"FAIL", _tCmd.iLine
          , _tCmd.strArg1.c_str(), strOp.c_str(), strRight.c_str(), strLeft.c_str());
    }

    if (!bPass)
        ++m_iExpectFail;
}

void CTestMgr::OnObjectCreated(CObj* _pObj, const LAYER _eLayer)
{
    if (!m_bActive)
        return;
    const wstring strClass = ClassName(_pObj);
    ++m_mapCreated[strClass];
    if (m_mapFirstCreated.find(strClass) == m_mapFirstCreated.end())
        m_mapFirstCreated[strClass] = m_iFrame;
    m_mapCreateFrame[_pObj->GetID()] = m_iFrame;
    if (m_iLogEvery > 0)
        Log(L"E %d create %s L%d id=%u", m_iFrame, strClass.c_str(), static_cast<int>(_eLayer), _pObj->GetID());
}

void CTestMgr::OnObjectDeleted(CObj* _pObj)
{
    if (!m_bActive)
        return;
    const wstring strClass = ClassName(_pObj);
    ++m_mapDeleted[strClass];
    if (m_mapFirstDeleted.find(strClass) == m_mapFirstDeleted.end())
    {
        m_mapFirstDeleted[strClass] = m_iFrame;
        m_mapDeletedFrm[strClass]   = AnimFrame(_pObj->GetAnimator());
        const auto iter = m_mapCreateFrame.find(_pObj->GetID());
        if (iter != m_mapCreateFrame.end())
            m_mapLifetime[strClass] = m_iFrame - iter->second;
    }
    m_mapCreateFrame.erase(_pObj->GetID());
    if (m_iLogEvery > 0)
        Log(L"E %d delete %s id=%u", m_iFrame, strClass.c_str(), _pObj->GetID());
}

void CTestMgr::OnLevelEnter(const LEVEL_TYPE _eLevel)
{
    if (!m_bActive)
        return;

    const wstring strLevel = LevelToStr(_eLevel);
    const int     iCount   = ++m_mapLevelEnter[strLevel];
    const long long llGdi  = GdiCount();
    const long long llMem  = PrivateBytes();

    wchar_t szKey[64] = {};
    swprintf_s(szKey, L"gdi.enter.%s.%d", strLevel.c_str(), iCount);
    m_mapStat[szKey] = llGdi;
    swprintf_s(szKey, L"mem.enter.%s.%d", strLevel.c_str(), iCount);
    m_mapStat[szKey] = llMem;

    Log(L"E %d level-enter %s #%d gdi=%lld mem=%lld", m_iFrame, strLevel.c_str(), iCount, llGdi, llMem);
}

void CTestMgr::OnError(const wchar_t* _szTitle, const wchar_t* _szMsg)
{
    ++m_iErrorCount;
    Log(L"ERROR %d [%s] %s", m_iFrame, _szTitle, _szMsg);
}

void CTestMgr::LogState()
{
    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
    if (nullptr == pLevel)
        return;

    int iObjCount = 0;
    for (UINT i = 0; i < static_cast<UINT>(LAYER::END); ++i)
        iObjCount += static_cast<int>(pLevel->GetLayer(static_cast<LAYER>(i)).size());

    const Vec2 vCamLT = CameraLT();
    CPlayer*   pPlayer = FindPlayer();
    if (nullptr == pPlayer)
    {
        Log(L"F %d L=%s cam=(%.2f,%.2f) objs=%d", m_iFrame, LevelToStr(CLevelMgr::GetInst()->GetCurLevelType()), vCamLT.x, vCamLT.y, iObjCount);
        return;
    }

    const Vec2 vPos = ScreenPos(pPlayer);
    Log(L"F %d L=%s cam=(%.2f,%.2f) P=(%.2f,%.2f) a=%s/%s life=%d bomb=%d bul=%d hmg=%d mon=%d objs=%d"
      , m_iFrame, LevelToStr(CLevelMgr::GetInst()->GetCurLevelType()), vCamLT.x, vCamLT.y, vPos.x, vPos.y
      , AnimName(pPlayer->GetAnimator()), AnimName(pPlayer->GetAnimator2())
      , pPlayer->GetLifeCount(), pPlayer->GetBombCount(), pPlayer->GetBulletCount(), pPlayer->IsHMG() ? 1 : 0
      , static_cast<int>(pLevel->GetLayer(LAYER::MONSTER).size()), iObjCount);
}

void CTestMgr::LogMonsters()
{
    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
    if (nullptr == pLevel)
        return;

    for (CObj* pObj : pLevel->GetLayer(LAYER::MONSTER))
    {
        CMonster*  pMon = dynamic_cast<CMonster*>(pObj);
        const Vec2 vPos = ScreenPos(pObj);
        Log(L"M %d %s id=%u (%.2f,%.2f) hp=%d a=%s/%s%s", m_iFrame, ClassName(pObj).c_str(), pObj->GetID(), vPos.x, vPos.y
          , pMon ? pMon->GetHP() : -1, AnimName(pObj->GetAnimator()), AnimName(pObj->GetAnimator2())
          , pObj->IsDead() ? L" dead" : L"");
    }
}

void CTestMgr::Capture(const wstring& _strName) const
{
    const HDC     hDC  = CEngine::GetInst()->GetMemTexDC();
    const HBITMAP hBmp = static_cast<HBITMAP>(GetCurrentObject(hDC, OBJ_BITMAP));
    BITMAP        tBmp = {};
    GetObject(hBmp, sizeof(BITMAP), &tBmp);

    BITMAPINFOHEADER tInfo = {};
    tInfo.biSize           = sizeof(BITMAPINFOHEADER);
    tInfo.biWidth          = tBmp.bmWidth;
    tInfo.biHeight         = tBmp.bmHeight;
    tInfo.biPlanes         = 1;
    tInfo.biBitCount       = 24;
    tInfo.biCompression    = BI_RGB;

    const int    iStride = ((tBmp.bmWidth * 3) + 3) & ~3;
    vector<BYTE> vecBits(static_cast<size_t>(iStride) * tBmp.bmHeight);
    GetDIBits(hDC, hBmp, 0, tBmp.bmHeight, vecBits.data(), reinterpret_cast<BITMAPINFO*>(&tInfo), DIB_RGB_COLORS);

    BITMAPFILEHEADER tFile = {};
    tFile.bfType           = 0x4D42;
    tFile.bfOffBits        = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    tFile.bfSize           = tFile.bfOffBits + static_cast<DWORD>(vecBits.size());

    const wstring strPath = m_strOutDir + L"\\" + m_strName + L"_" + _strName + L".bmp";
    FILE*         pFile   = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"wb");
    if (nullptr == pFile)
        return;
    fwrite(&tFile, sizeof(tFile), 1, pFile);
    fwrite(&tInfo, sizeof(tInfo), 1, pFile);
    fwrite(vecBits.data(), 1, vecBits.size(), pFile);
    fclose(pFile);
}

void CTestMgr::Log(const wchar_t* _szFormat, ...)
{
    if (nullptr == m_pLog)
        return;

    va_list args;
    va_start(args, _szFormat);
    vfwprintf_s(m_pLog, _szFormat, args);
    va_end(args);
    fputwc(L'\n', m_pLog);
}

void CTestMgr::Finish()
{
    if (m_bFinished)
        return;

    if (EXIT_PASS == m_iExitCode && m_iExpectFail > 0)
        m_iExitCode = EXIT_EXPECT_FAIL;

    Log(L"RESULT %s exit=%d frames=%d expect=%d/%d errors=%d allowleaks=%d gdi=%lld tick=%.3fms render=%.3fms"
      , m_bSkipped ? L"SKIP" : (EXIT_PASS == m_iExitCode ? L"PASS" : L"FAIL"), m_iExitCode, m_iFrame + 1
      , m_iExpectCount - m_iExpectFail, m_iExpectCount, m_iErrorCount, m_bAllowLeaks ? 1 : 0, GdiCount()
      , m_iTimedFrames > 0 ? m_dTickMsSum / m_iTimedFrames : 0.0, m_iTimedFrames > 0 ? m_dRenderMsSum / m_iTimedFrames : 0.0);

    if (nullptr != m_pLog)
        fflush(m_pLog);

    m_bFinished = true;

    // 이제 프로세스가 끝나야 한다. 끝나지 않으면 감시 스레드가 위치를 남기고 끝낸다
    g_dwMainThreadId = GetCurrentThreadId();
    const HANDLE hThread = CreateThread(nullptr, 0, ExitWatchdog, nullptr, 0, nullptr);
    if (nullptr != hThread)
        CloseHandle(hThread);
}

void CTestMgr::Abort(const int _iExitCode, const wchar_t* _szReason)
{
    Log(L"ABORT frame=%d reason=%s", m_iFrame, _szReason);
    m_iExitCode = _iExitCode;
    m_bFinished = false;
    Finish();
    TerminateProcess(GetCurrentProcess(), static_cast<UINT>(_iExitCode));
}

wstring CTestMgr::MakeOutPath(const wchar_t* _szExt) const
{
    return m_strOutDir + L"\\" + m_strName + _szExt;
}

void ReportError(const wchar_t* _szTitle, const wchar_t* _szMsg)
{
    if (CTestMgr::GetInst()->IsActive())
    {
        CTestMgr::GetInst()->OnError(_szTitle, _szMsg);
        return;
    }
    MessageBox(nullptr, _szMsg, _szTitle, MB_OK);
}

const wchar_t* LevelToStr(const LEVEL_TYPE _eLevel)
{
    switch (_eLevel)
    {
    case LEVEL_TYPE::GAMEOVER:   return L"GAMEOVER";
    case LEVEL_TYPE::START:      return L"START";
    case LEVEL_TYPE::STAGE:      return L"STAGE";
    case LEVEL_TYPE::ANIMEDITOR: return L"ANIMEDITOR";
    default:                     return L"NONE";
    }
}

bool StrToLevel(const wchar_t* _szName, LEVEL_TYPE& _eLevel)
{
    for (UINT i = 0; i < static_cast<UINT>(LEVEL_TYPE::END); ++i)
    {
        if (0 == wcscmp(_szName, LevelToStr(static_cast<LEVEL_TYPE>(i))))
        {
            _eLevel = static_cast<LEVEL_TYPE>(i);
            return true;
        }
    }
    return false;
}
