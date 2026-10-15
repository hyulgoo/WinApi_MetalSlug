#pragma once

class CTimeMgr
{
	SINGLE(CTimeMgr)
private:
	LARGE_INTEGER	m_llPrevCount;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER	m_llFrequency;

	UINT			m_iCallCount;
	float			m_fDeltaTime;
	float			m_fTime;
	bool			m_bTimeStop;

public:
	void			init();
	void			tick();
	void			render();

public:
	float			GetDeltaTime() const { return m_fDeltaTime; }
	void			TimeStop(bool _Stop);
	void			SetTimeStop() { m_bTimeStop = !m_bTimeStop; }
	void			ResetTimeStop() { m_bTimeStop = false; }	// 레벨을 바꿀 때 (에디터의 시간 정지가 넘어가지 않게)
};