#include "pch.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

CTimeMgr::CTimeMgr()
    : m_llPrevCount{}
    , m_llCurCount{}
    , m_llFrequency{}
    , m_iCallCount(0)
    , m_fDeltaTime(0.f)
    , m_fRealTime(0.f)
    , m_fMaxDT(0.f)
    , m_fTargetFPS(0.f)
{
}

CTimeMgr::~CTimeMgr()
{
    timeEndPeriod(1);
}

void CTimeMgr::init()
{
    // 프레임 제한에서 Sleep이 1ms 단위로 깨어나게 한다
    timeBeginPeriod(1);

    const CDataSection& tTime = CDataMgr::GetInst()->GetSection(L"game", L"time");
    m_fTargetFPS              = tTime.GetFloat(L"fps");
    m_fMaxDT                  = 1.f / tTime.GetFloat(L"min_fps");

    // 1초당 카운트 수를 구함
    QueryPerformanceFrequency(&m_llFrequency);
    QueryPerformanceCounter(&m_llCurCount);
    QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{
    QueryPerformanceCounter(&m_llCurCount);

    // tick 사이 시간 (자동 테스트는 실제 시간과 상관없이 고정값)
    const float fRealDT = static_cast<float>(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / static_cast<float>(m_llFrequency.QuadPart);
    m_fDeltaTime        = CTestMgr::GetInst()->IsActive() ? CTestMgr::GetInst()->GetFixedDT() : fRealDT;

    // 한 프레임이 너무 길면 게임 시간을 늦춘다 (지형 뚫기 방지)
    m_fDeltaTime = min(m_fDeltaTime, m_fMaxDT);

    // FPS 표시는 실제 시간 기준
    m_fRealTime += fRealDT;

    // 함수 호출 횟수
    ++m_iCallCount;

    // 이전 카운트 값을 현재 카운트로 갱신
    m_llPrevCount = m_llCurCount;
}

void CTimeMgr::WaitNextFrame() const
{
    // 자동 테스트는 기다리지 않고 바로 다음 프레임을 돈다
    if (CTestMgr::GetInst()->IsActive())
        return;

    const LONGLONG llFrameCount = static_cast<LONGLONG>(static_cast<double>(m_llFrequency.QuadPart) / m_fTargetFPS);
    const LONGLONG llTarget     = m_llCurCount.QuadPart + llFrameCount;

    LARGE_INTEGER llNow = {};
    QueryPerformanceCounter(&llNow);
    while (llNow.QuadPart < llTarget)
    {
        // 2ms 넘게 남았으면 자고, 그보다 짧으면 잠깐씩 양보하며 기다린다 (Sleep은 1ms 단위)
        const double dRemainMs = static_cast<double>(llTarget - llNow.QuadPart) * 1000.0 / static_cast<double>(m_llFrequency.QuadPart);
        if (dRemainMs > 2.0)
            Sleep(static_cast<DWORD>(dRemainMs - 1.0));
        else
            YieldProcessor();
        QueryPerformanceCounter(&llNow);
    }
}


void CTimeMgr::UpdateWindowTitle()
{
    if (1.f <= m_fRealTime)
    {
		wchar_t szBuff[256] = {};

		swprintf_s(szBuff, L"Metal Slug");

#ifdef _DEBUG
        swprintf_s(szBuff, L"FPS : %d, DT : %f", m_iCallCount, m_fDeltaTime);
#endif // _DEBUG
        SetWindowText(CEngine::GetInst()->GetMainWnd(), szBuff);

        m_fRealTime  = 0.f;
        m_iCallCount = 0;
    }
}

