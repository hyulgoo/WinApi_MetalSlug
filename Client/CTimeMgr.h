#pragma once

class CTimeMgr
{
	SINGLE(CTimeMgr);
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
	float			GetDeltaTime() { return m_fDeltaTime; }
	void			TimeStop(bool _Stop);
	void			SetTimeStop() { m_bTimeStop = m_bTimeStop ? false : true; }
};