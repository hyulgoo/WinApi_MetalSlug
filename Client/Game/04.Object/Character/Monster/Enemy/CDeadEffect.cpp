#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"

#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

CDeadEffect::CDeadEffect()
	: m_pAtlas(nullptr)
	, m_bDir(false)
	, m_bStart(false)
	, m_bFire(false)
	, m_iType(1)
{
	CreateAnimator();
	CreatePixelCollider();
	CreateRigidbody();
	m_irand = rand() % 2;
}

CDeadEffect::~CDeadEffect()
{
}

void CDeadEffect::tick()
{
	// 첫 tick에 애니메이션을 고른다 (예전에는 누적 시간이 0인지로 봐서, 시간 정지 중에는 매 tick 다시 불러왔다)
	if (!m_bStart)
	{
		m_bStart = true;
		if (m_iType == 1)
			m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Soldier", L"texture\\Soldier.bmp");
		else if (m_iType == 2)
			m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Zombie", L"texture\\Zombie.bmp");
		else if (m_iType < 1 || m_iType > 2)
			assert(false);
		LoadAnimation();
		Play(m_bDir);
	}


	CObj::tick();

	// 애니메이션이 끝나면 사라진다. 예전에는 애니메이션 길이를 따로 적은 타이머(병사 1.1·2.2초, 좀비 0.6·4.62초)로
	// 지워서, 재생 속도가 FPS에 따르던 때에는 애니메이션이 잘렸다. 좀비 일반 사망은 0.6초로 0.75초짜리를 잘랐다
	if (GetAnimator()->IsFinish() && !IsDead())
		SetDead();
}

void CDeadEffect::render(const HDC _dc)
{
	CObj::render(_dc);
}

void CDeadEffect::LoadAnimation()
{
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD.anim");
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD_FIRE.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_DEAD2_LEFT.anim"); // LEFT
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_NORMAL.anim");
	GetAnimator()->LoadAnimation(L"Enemy//ZB_DEAD_FIRE.anim");
}

void CDeadEffect::PlayAnim(const wstring& _strName, const bool _bRepeat, const bool _bDirRight)
{
	GetAnimator()->Play(_strName, _bRepeat);
	if (_bDirRight == true)
		GetAnimator()->GetAnimation()->SetPlayRight();
	else if (_bDirRight == false)
		GetAnimator()->GetAnimation()->SetPlayLeft();
}

void CDeadEffect::Play(const bool _bDir)
{
	const bool bLeft = !_bDir;
	if (m_iType == 1)
	{
		if (m_bFire)
		{
			PlayAnim(L"SOLDIER_DEAD_FIRE.anim", false, bLeft);
		}
		else
		{
			if(m_irand == 0)
				PlayAnim(L"SOLDIER_DEAD.anim", false, bLeft);
			else if(m_irand == 1)
				PlayAnim(L"SOLDIER_DEAD2_LEFT.anim", false, bLeft);
		}
	}
	else if (m_iType == 2)
	{
		if (m_bFire)
		{
			PlayAnim(L"ZB_DEAD_FIRE.anim", false, bLeft);
		}
		else
		{
			PlayAnim(L"ZB_DEAD_NORMAL.anim", false, bLeft);
		}
	}
}

