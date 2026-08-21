#include "pch.h"
#include "Game\04.Object\Bullet\Effect\CSLDBombEffect.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"


CSLDBombEffect::CSLDBombEffect()
    : m_bDir()
    , m_bState()
    , m_fDeadTime()
    , m_fTime()
    , m_bStart(false)

{
    CreateAnimator();

    m_pTexture = CResMgr::GetInst()->LoadTexture(L"SoldierSFX", L"texture\\SFX.bmp");
    GetAnimator()->LoadAnimation(L"Enemy//SOLDIER_BOMB_SFX.anim");
}

CSLDBombEffect::~CSLDBombEffect()
{
}

void CSLDBombEffect::tick()
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

    if (m_bStart == false)
    {
        if (m_bState)
        {
            m_fDeadTime = 0.3f;
            m_bStart    = true;
            GetAnimator()->Play(L"SOLDIER_BOMB_SFX.anim", false);
            if (m_bDir)
                GetAnimator()->GetAnimation()->SetPlayRight();
            else
                GetAnimator()->GetAnimation()->SetPlayLeft();
        }
        //else if (m_bState == false)
        //{
        //	m_fDeadTime = 0.84f;
        //	m_bStart = true;
        //	GetAnimator()->Play(L"ZB_PROJECTILE_HIT.anim", false);
        //	if (m_bDir)
        //		GetAnimator()->GetAnimation()->SetPlayRight();
        //	else
        //		GetAnimator()->GetAnimation()->SetPlayLeft();
        //}
    }
    CObj::tick();

    m_fTime += DT;
    if (m_fTime > m_fDeadTime && !IsDead())
    {
        SetDead();
    }
}

void CSLDBombEffect::render(const HDC _dc)
{
    CObj::render(_dc);
}
