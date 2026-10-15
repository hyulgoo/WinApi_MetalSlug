#include "pch.h"
#include "Game\04.Object\Bullet\Effect\CWeaponEffect.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CWeaponEffect::CWeaponEffect() 
	: m_bDir()
	, m_bStart(false)
	, m_eKind(KIND::GROUND)
{
	CreateAnimator();

	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT.anim");
	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT_HIT.anim");
	GetAnimator()->LoadAnimation(L"Player//BULLET_EFFECT_WALL.anim");
	GetAnimator()->LoadAnimation(L"Player//BOMB_SFX.anim");
}

CWeaponEffect::~CWeaponEffect()
{
}

void CWeaponEffect::tick()
{
	if (!m_bStart)
	{
		m_bStart = true;

		// 같은 자리에 겹쳐도 똑같아 보이지 않게 방향과 위치를 조금 섞는다(data\weapon.ini [effect]). 벽 이펙트는 늘 왼쪽을 본다
		const int   iRandom = rand() % 2;
		const float fJitter = CDataMgr::GetInst()->GetSection(L"weapon", L"effect").GetFloat(L"jitter_x");
		SetPos(Vec2(GetPos().x + static_cast<float>(iRandom) * fJitter, GetPos().y));
		m_bDir = (0 == iRandom) && KIND::WALL != m_eKind;

		const wchar_t* arrAnim[] = { L"BULLET_EFFECT.anim", L"BULLET_EFFECT_HIT.anim", L"BULLET_EFFECT_WALL.anim", L"BOMB_SFX.anim" };
		GetAnimator()->Play(arrAnim[static_cast<int>(m_eKind)], false);
		if (m_bDir)
			GetAnimator()->GetAnimation()->SetPlayRight();
		else
			GetAnimator()->GetAnimation()->SetPlayLeft();
	}

	CObj::tick();

	// 애니메이션이 끝나면 사라진다. 예전에는 애니메이션 길이를 따로 적은 타이머(0.3초, 폭발 0.5초)로 지워서,
	// 재생 속도가 FPS에 따르던 때에는 애니메이션이 잘리거나 마지막 프레임에 멈춰 있었다
	if (GetAnimator()->IsFinish() && !IsDead())
		SetDead();
}

void CWeaponEffect::render(const HDC _dc)
{
	CObj::render(_dc);
}
