#include "pch.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"


CCamera::CCamera()
	: m_pBlindTex(nullptr)
	, m_fRatio(0.f)
	, m_fAccTime(0.f)
	, m_fMaxTime(0.f)
	, m_fRange(0.f)
	, m_fShakeSpeed(0.f)
	, m_fShakeDir(0.f)
	, m_bCamShake(false)
{
	m_vResolution = CEngine::GetInst()->GetResolution();
	m_pBlindTex = CResMgr::GetInst()->CreateTexture(L"m_BindTex", static_cast<int>(m_vResolution.x), static_cast<int>(m_vResolution.y));

	// 처음에는 월드 좌표 = 화면 좌표 (Reset과 같다)
	m_vLook = m_vResolution / 2.f;
	UpdateDiff();
}

CCamera::~CCamera()
{
}

void CCamera::tick()
{
	CameraEffect();
	CameraShake();
	UpdateDiff();
}

void CCamera::UpdateDiff()
{
	// 바라보는 곳이나 흔들림이 바뀌면 바로 다시 구한다. 예전에는 tick 시작에 한 번만 구해서,
	// 같은 프레임에 플레이어가 카메라를 민 것은 다음 프레임에야 배경에 반영됐다
	m_vDiff = m_vLook - m_vResolution / 2.f + m_vOffset;
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
	tBlend.SourceConstantAlpha = static_cast<BYTE>(255.f * m_fRatio);

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

void CCamera::Reset()
{
	// 레벨을 바꿀 때 이전 레벨의 효과가 넘어가지 않게 한다
	m_CamEffectList.clear();
	m_fRatio      = 0.f;
	m_bCamShake   = false;
	m_fAccTime    = 0.f;
	m_fMaxTime    = 0.f;
	m_vOffset     = Vec2(0.f, 0.f);

	// 카메라를 쓰지 않는 레벨은 월드 좌표 = 화면 좌표가 되게 처음 자리로 (스테이지는 Enter에서 다시 정한다)
	m_vLook = m_vResolution / 2.f;
	UpdateDiff();
}

void CCamera::FadeIn(const float _fTerm)
{
	tCamEffect effect = {};
	effect.m_eCurEffect = CAMERA_EFFECT::FADE_IN;
	effect.m_fAccTime = 0.f;
	effect.m_fMaxTime = _fTerm;

	m_CamEffectList.push_back(effect);
}

void CCamera::FadeOut(const float _fTerm)
{
	tCamEffect effect = {};
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

	// AccTime이 MaxTime을 넘었다면 효과가 끝난 것이므로 끝값을 적용하고 처리된 입력을 제거한다
	// (끝값을 맞추지 않으면 페이드인 뒤에도 거의 투명한 AlphaBlend가 매 프레임 남는다)
	if (effect.m_fMaxTime <= effect.m_fAccTime)
	{
		m_fRatio = CAMERA_EFFECT::FADE_IN == effect.m_eCurEffect ? 0.f : 1.f;
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
		return;
	}

	m_vOffset.x += DT * m_fShakeSpeed * m_fShakeDir;

	if (m_fRange < fabsf(m_vOffset.x))
	{
		m_vOffset.x = m_fRange * m_fShakeDir;
		m_fShakeDir *= -1;
	}
}
