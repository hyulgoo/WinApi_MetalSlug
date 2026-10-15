#pragma once

class CTexture;

struct tCamEffect
{
	CAMERA_EFFECT		m_eCurEffect = CAMERA_EFFECT::NONE;
	float				m_fAccTime   = 0.f;
	float				m_fMaxTime   = 0.f;
};

class CCamera
{
	SINGLE(CCamera);

private:
	Vec2				m_vLook;	// 화면 가운데가 보는 월드 위치
	Vec2				m_vDiff;	// 화면 왼쪽 위의 월드 위치 (흔들림 포함)
	CTexture*			m_pBlindTex;
	list<tCamEffect>	m_CamEffectList;
	float				m_fRatio;

	Vec2				m_vOffset;
	float				m_fAccTime;
	float				m_fMaxTime;
	float				m_fRange;
	float				m_fShakeSpeed;
	float				m_fShakeDir;
	bool				m_bCamShake;

	Vec2				m_vResolution;

public:
	void tick();
	void final_tick() const;
	void render(HDC _dc) const;

public:
	void  SetLook(const Vec2& _vLook) { m_vLook = _vLook; UpdateDiff(); }
	Vec2  GetLook() const { return m_vLook; }

	void Reset();
	void FadeIn(float _fTerm);
	void FadeOut(float _fTerm);
	void CameraShake(float _fRange, float _fShakeSpeed, float _fTerm);
	void SetOffset(const Vec2& _vOffset) { m_vOffset = _vOffset; UpdateDiff(); }
	Vec2 GetOffset() const { return m_vOffset; }
	float GetFadeRatio() const { return m_fRatio; }

public:
	// 월드 좌표 -> 화면 좌표, 화면 좌표 -> 월드 좌표.
	// 오브젝트는 월드 좌표로 움직이고 그릴 때만 화면으로 바꾼다 (CObj::GetScreenPos)
	Vec2 GetRenderPos(const Vec2& _vRealPos) const { return _vRealPos - m_vDiff; }
	Vec2 GetRealPos(const Vec2& _vRenderPos) const { return _vRenderPos + m_vDiff; }

private:
	void				CameraEffect();
	void				CameraShake();
	void				UpdateDiff();
};

