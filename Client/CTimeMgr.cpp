#include "pch.h"
#include "CTimeMgr.h"

#include "CEngine.h"

CTimeMgr::CTimeMgr()
    : m_llPrevCount{}
    , m_llCurCount{}
    , m_llFrequency{}
    , m_iCallCount(0)
    , m_fDeltaTime(0.f)
    , m_fTime(0.f)
    , m_bTimeStop(false)
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::init()
{
    // 1초당 카운트 수를 구함
    QueryPerformanceFrequency(&m_llFrequency);
    QueryPerformanceCounter(&m_llCurCount);
    QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{
    QueryPerformanceCounter(&m_llCurCount);

    // tick 사이 시간
    m_fDeltaTime = static_cast<float>(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / static_cast<float>(m_llFrequency.QuadPart);

    // 누적 시간
    m_fTime += m_fDeltaTime;
    TimeStop(m_bTimeStop);

    // 함수 호출 횟수
    ++m_iCallCount;

    // 이전 카운트 값을 현재 카운트로 갱신
    m_llPrevCount = m_llCurCount;
}


void CTimeMgr::render()
{
    if (1.f <= m_fTime)
    {
		wchar_t szBuff[256] = {};

		swprintf_s(szBuff, L"Metal Slug");

#ifdef _DEBUG
        swprintf_s(szBuff, L"FPS : %d, DT : %f", m_iCallCount, m_fDeltaTime);
#endif // _DEBUG
        SetWindowText(CEngine::GetInst()->GetMainWnd(), szBuff);

        m_fTime      = 0.f;
        m_iCallCount = 0;
    }
}

void CTimeMgr::TimeStop(const bool _Stop)
{
    if (_Stop == true)
    {
        m_fTime      = 0.f;
        m_fDeltaTime = 0.f;
        m_iCallCount = 0;
    }
}
