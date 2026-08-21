#pragma once

class CTexture;

struct tCamEffect
{
	CAMERA_EFFECT		m_eCurEffect;
	float				m_fAccTime;
	float				m_fMaxTime;
};

class CCamera
{
	SINGLE(CCamera);

private:
	Vec2				m_vLook;
	Vec2				m_vDiff;
	float				m_fSpeed;
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
	float				m_fMoveTime;
	float				m_fMoveDuration;
	bool				m_bMoveUpDown;

	Vec2				m_vResolution;

	Vec2				m_vPrevLook;
	Vec2				m_vCurLook;
	Vec2				m_vPastLook;
	bool				m_bLeftStop;
	bool				m_bRightStop;
	bool				m_bCameraStop;
	bool				m_bMoveCamera;
	float				m_fMoveSpeed;

public:
	void tick();
	void final_tick() const;
	void render(HDC _dc) const;

public:
	void  SetLook(const Vec2& _vLook) { m_vLook = _vLook; }
	Vec2  GetLook() { return m_vLook; }
	float GetCameraSpeed() const { return m_fSpeed; }
	void  SetCameraSpeed(const float _fSpeed) { m_fSpeed = _fSpeed; }
	void  MoveCamera(bool _bUpDown, float _fMoveTime, float _fSpeed);
	bool  GetIsMove() const { return m_bMoveCamera; }
	float GetMoveSpeed() const { return m_fMoveSpeed;}
	bool  GetMoveUpDown() const { return m_bMoveUpDown; }

	void FadeIn(float _fTerm);
	void FadeOut(float _fTerm);
	void CameraShake(float _fRange, float _fShakeSpeed, float _fTerm);
	bool IsCameraStop() const { return m_bCameraStop; }
	void SetCameraStop(const bool _bool) { m_bCameraStop = _bool; }
	void SetOffset(const Vec2& _vOffset) { m_vOffset = _vOffset; }

public:
	Vec2 GetRenderPos(const Vec2& _vRealPos) const { return _vRealPos - m_vDiff; }
	Vec2 GetRealPos(const Vec2& _vRenderPos) const { return _vRenderPos + m_vDiff; }

private:
	void				CameraEffect();
	void				CameraShake();
	void				MoveCamera();
};

