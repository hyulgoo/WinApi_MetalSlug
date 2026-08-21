#include "pch.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"


CCamera::CCamera()
	: m_fSpeed(330.f)
	, m_pBlindTex(nullptr)
	, m_fRatio(0.f)
	, m_fAccTime(0.f)
	, m_fMaxTime(0.f)
	, m_fRange(0.f)
	, m_fShakeSpeed(0.f)
	, m_fShakeDir(0.f)
	, m_bCamShake(false)
	, m_bMoveCamera(false)
	, m_fMoveSpeed(0.f)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_pBlindTex = CResMgr::GetInst()->CreateTexture(L"m_BindTex", static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y));
}

CCamera::~CCamera()
{
}

void CCamera::tick()
{ 
	m_vPrevLook = m_vLook;
	const Vec2 vResolution = m_vResolution / 2.f;
	// if (m_vLook.x >= vResolution.x)
	// {
	// 	if (IsPressed(KEY::J))
	// 		m_vLook.x -= m_fSpeed * DT;
	// }
	//
	// if (IsPressed(KEY::L))
	// 	m_vLook.x += m_fSpeed * DT;
	//
	// if (m_vLook.y >= vResolution.y)
	// {
	// 	if (IsPressed(KEY::I))
	// 		m_vLook.y -= m_fSpeed * DT;
	// }
	//
	// if (IsPressed(KEY::K))
	// 	m_vLook.y += m_fSpeed * DT;
	
	// if ((m_vLook.x + vResolution.x <= 6443))
	// {
	//	 if (IsPressed(KEY::RIGHT) && m_bCameraStop == false)
	//	 {
	//		 m_vLook.x += m_fSpeed * DT;
	//	 }
	// }
	// else
	//	 m_bCameraStop = true;

	m_vDiff = m_vLook - vResolution + m_vOffset;

	CameraEffect();

	CameraShake();

	MoveCamera();
}

void CCamera::final_tick() const
{	
}

void CCamera::render(const HDC _dc) const
{
	BLENDFUNCTION tBlend = {};

	tBlend.AlphaFormat = 0;
	tBlend.BlendFlags = 0;
	tBlend.BlendOp = AC_SRC_OVER;
	tBlend.SourceConstantAlpha = static_cast<int>(255.f * m_fRatio);

	if (m_fRatio < 0.0001f)
		return;

	AlphaBlend(_dc
		, 0, 0
		, static_cast<int>(m_pBlindTex->Width())
		, static_cast<int>(m_pBlindTex->Height())
		, m_pBlindTex->GetDC()
		, 0, 0
		, static_cast<int>(m_pBlindTex->Width())
		, static_cast<int>(m_pBlindTex->Height())
		, tBlend);
}

void CCamera::MoveCamera(const bool _bUpDown, const float _fMoveTime, const float _fSpeed)
{
	m_bMoveUpDown = _bUpDown;
	m_fMoveDuration = _fMoveTime;
	m_fMoveSpeed = _fSpeed;
	m_bMoveCamera = true;
}

void CCamera::FadeIn(const float _fTerm)
{
	tCamEffect effect;
	effect.m_eCurEffect = CAMERA_EFFECT::FADE_IN;
	effect.m_fAccTime = 0.f;
	effect.m_fMaxTime = _fTerm;

	m_CamEffectList.push_back(effect);
}

void CCamera::FadeOut(const float _fTerm)
{
	tCamEffect effect;
	effect.m_eCurEffect = CAMERA_EFFECT::FADE_OUT;
	effect.m_fAccTime = 0.f;
	effect.m_fMaxTime = _fTerm;

	m_CamEffectList.push_back(effect);
}

void CCamera::CameraShake(const float _fRange, const float _fShakeSpeed, const float _fTerm)
{
	m_fAccTime = 0.f;
	m_fMaxTime = _fTerm;
	m_fRange = _fRange;
	m_fShakeSpeed = _fShakeSpeed;
	m_fShakeDir = 1.f;
	m_bCamShake = true;
}

void CCamera::CameraEffect()
{
	// CameraEffect가 아무 입력도 없다면 return
	if (m_CamEffectList.empty())
		return;

	// 있다면 front()를 통해 가장 먼저 들어온 입력부터 처리한다.
	tCamEffect& effect = m_CamEffectList.front();

	effect.m_fAccTime += DT;

	// AccTime이 MaxTime을 넘었다면 효과가 끝난 것이므로 pop_front로 처리된 입력 제거
	if (effect.m_fMaxTime <= effect.m_fAccTime)
	{
		m_CamEffectList.pop_front();
		return;
	}

	m_fRatio = effect.m_fAccTime / effect.m_fMaxTime;
	if (CAMERA_EFFECT::FADE_IN == effect.m_eCurEffect)
		m_fRatio = 1.f - m_fRatio;

}

void CCamera::CameraShake()
{
	if (!m_bCamShake)
		return;

	m_fAccTime += DT;

	if (m_fMaxTime <= m_fAccTime)
	{
		m_bCamShake = false;
		m_vOffset = Vec2(0.f, 0.f);
	}

	m_vOffset.x += DT * m_fShakeSpeed * m_fShakeDir;

	if (m_fRange < fabsf(m_vOffset.x))
	{
		m_vOffset.x = m_fRange * m_fShakeDir;
		m_fShakeDir *= -1;
	}
}

void CCamera::MoveCamera()
{
	if (!m_bMoveCamera)
		return;
	m_fMoveTime += DT;

	if (m_fMoveTime >= m_fMoveDuration)
	{
		m_bMoveCamera = false;
		m_fMoveTime = 0.f;
	}
	else
	{
		if (m_bMoveUpDown)
			m_vOffset -= Vec2(0.f, m_fMoveSpeed * 100.f) * DT;
		else
			m_vOffset += Vec2(0.f, m_fMoveSpeed * 100.f) * DT;
	}
}
