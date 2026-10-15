#include "pch.h"
#include "Game\04.Object\Bullet\Effect\CZB_PRJ_Effect.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"

CZB_PRJ_Effect::CZB_PRJ_Effect()
    : m_bDir()
    , m_bState()
    , m_bStart(false)

{
    CreateAnimator();

    m_pTexture = CResMgr::GetInst()->FindTexture(L"Zombie");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_GROUND.anim");
    GetAnimator()->LoadAnimation(L"Enemy//ZB_PROJECTILE_HIT.anim");
}

CZB_PRJ_Effect::~CZB_PRJ_Effect()
{
}

void CZB_PRJ_Effect::tick()
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4.f * DT, GetPos().y));

    if (m_bStart == false)
    {
        if (m_bState)
        {
            m_bStart    = true;
            GetAnimator()->Play(L"ZB_PROJECTILE_GROUND.anim", false);
            if (m_bDir)
                GetAnimator()->GetAnimation()->SetPlayRight();
            else
                GetAnimator()->GetAnimation()->SetPlayLeft();
        }
        else if (m_bState == false)
        {
            m_bStart    = true;
            GetAnimator()->Play(L"ZB_PROJECTILE_HIT.anim", false);
            if (m_bDir)
                GetAnimator()->GetAnimation()->SetPlayRight();
            else
                GetAnimator()->GetAnimation()->SetPlayLeft();
        }
    }
    CObj::tick();

    // 애니메이션이 끝나면 사라진다. 예전에는 애니메이션 길이를 따로 적은 타이머(땅 1.5초, 명중 0.84초)로 지워서,
    // 재생 속도가 FPS에 따르던 때에는 애니메이션이 잘리거나 마지막 프레임에 멈춰 있었다
    if (GetAnimator()->IsFinish() && !IsDead())
        SetDead();
}

void CZB_PRJ_Effect::render(const HDC _dc)
{
    CObj::render(_dc);
}
