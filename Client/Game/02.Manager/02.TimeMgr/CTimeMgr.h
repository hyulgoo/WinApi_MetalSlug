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
	float			m_fRealTime;	// FPS 표시용 실제 경과 시간

public:
	// 한 프레임의 게임 시간 상한. 창을 끌거나 브레이크포인트에 걸려 DT가 수 초가 되면
	// 오브젝트가 지형을 뚫고 지나가므로, 그보다 긴 프레임은 게임 시간을 늦춘다
	static constexpr float MAX_DT     = 1.f / 30.f;
	// 프레임 제한 (원작과 같은 60 FPS). 코어 하나를 100% 쓰지 않게 남는 시간은 쉰다
	static constexpr float TARGET_FPS = 60.f;

public:
	void			init();
	void			tick();
	void			UpdateWindowTitle();	// 1초마다 창 제목을 바꾼다 (Debug는 FPS·DT)
	void			WaitNextFrame() const;	// 프레임 끝에서 다음 프레임 시작 시각까지 기다린다

public:
	float			GetDeltaTime() const { return m_fDeltaTime; }
};
