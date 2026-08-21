#include "pch.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

#include <algorithm>

#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\04.Object\Bullet\Player\CHandgun.h"
#include "Game\04.Object\Bullet\Player\CPlayerCQC.h"
#include "Game\04.Object\Bullet\Player\CBomb.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\04.Object\Bullet\Player\CCQCArea.h"

#include "Game\01.Engine\CEngine.h"

CPlayer::CPlayer()
    : m_arrDir{}
    , m_fSpeed(330.f)
    , m_fInvincibleTime(0.f)
    , m_fPlayerDir()
    , m_fCameraRatio(0.45f)
    , m_vZero{}
    , m_bHMG(true)
    , m_tDir(DIR::NONE)
    , m_tUpperState(STATE::IDLE)
    , m_tLowerState(STATE::IDLE)
    , m_tHRZ_Dir(DIR::RIGHT)
    , m_tJumpBow(UPDOWN::IDLE)
    , m_iBulletCount(20)
    , m_iBombCount(10)
    , m_iLiftCount(3)
    , m_bSumon(true)
    , m_bStart(true)
    , m_bGod(true)
{
    CreateCollider();
    CreateAnimator();
    CreateAnimator2();
    CreateRigidbody();
    CreatePixelCollider();

    GetRigidbody()->SetGravityAccel(1500.f);
    SetScale(Vec2(80.f, 120.f));

    m_vResolution = CEngine::GetInst()->GetResolution();
    m_pAtlas      = CResMgr::GetInst()->LoadTexture(L"Player", L"texture\\Eri.bmp");

    LoadAnim();

    GetRigidbody()->SetVelocityLimit(200.f);
    GetRigidbody()->SetFriction(500.f);
    GetRigidbody()->SetGravity(true);
    GetRigidbody()->SetGravityAccel(1000.f);

    m_pCQC = new CCQCArea;
    m_pCQC->SetPos(GetPos());
    Instantiate(m_pCQC, GetPos(), LAYER::CQC);
}

CPlayer::~CPlayer()
{
}

void CPlayer::tick()
{
    CObj::tick();

    if (m_bSumon)
    {
        if (m_bStart)
        {
            PlayAnim(L"SUMON.anim", SETPRINT::NORMAL, false, true);
            m_bStart = false;
        }
        m_fSumonTime += DT;
        if (m_fSumonTime > 0.43f)
        {
            m_bSumon = false;
            PlayLowerAnim();
            PlayUpperAnim();
        }
        return;
    }

    if (IsTap(KEY::F))
        PlayAnim(L"VICTORY.anim", SETPRINT::NORMAL, true, true);
    if (IsTap(KEY::V))
        SetGod();

    m_tShiftUpperState = m_tUpperState;
    m_tShiftLowerState = m_tLowerState;
    m_vPrevPos         = GetPos();
    m_tShiftHRZDir     = m_tHRZ_Dir;
    m_tShiftVRTDir     = m_tVRT_Dir;
    m_tShiftJumpBow    = m_tJumpBow;
    if (!m_bDead)
    {
        RenewalPos();
        RenewalDir();
    }
    Vec2 vOFS = {};
    if (m_tHRZ_Dir == DIR::LEFT)
    {
        vOFS = Vec2(-50.f, 0.f);
    }
    else if (m_tHRZ_Dir == DIR::RIGHT)
    {
        vOFS = Vec2(50.f, 0.f);
    }
    m_pCQC->SetPos(GetPos() + vOFS);
    m_bCQC = m_pCQC->GetCQC();

    // 애니메이션 재생 조건이 되면 재생하기
    // 이전 상태와 현재 상태(하체, 상체, 방향, 점프여부) 중 하나라도 바뀌면 다시 재생한다.
    if (m_tShiftLowerState != m_tLowerState || m_tShiftJumpBow != m_tJumpBow
        || m_tShiftHRZDir != m_tHRZ_Dir || m_tShiftVRTDir != m_tVRT_Dir)
    {
        if (m_tShiftLowerState != m_tLowerState)
            m_tPrevLowerState = m_tShiftLowerState;
        if (m_tShiftHRZDir != m_tHRZ_Dir)
            m_tPrevHRZDir = m_tShiftHRZDir;
        if (m_tShiftJumpBow != m_tJumpBow)
            m_tPrevJumpBow = m_tShiftJumpBow;
        PlayLowerAnim();
    }
    if (m_tShiftUpperState != m_tUpperState || m_tShiftJumpBow != m_tJumpBow
        || m_tShiftHRZDir != m_tHRZ_Dir || m_tShiftVRTDir != m_tVRT_Dir)
    {
        if (m_tShiftUpperState != m_tUpperState)
            m_tPrevUpperState = m_tShiftUpperState;
        if (m_tShiftHRZDir != m_tHRZ_Dir)
            m_tPrevHRZDir = m_tShiftHRZDir;
        if (m_tShiftJumpBow != m_tJumpBow)
            m_tPrevJumpBow = m_tShiftJumpBow;
        if (m_bAttackOn == false && m_bBombOn == false)
            PlayUpperAnim();
    }

    // 총알 발사위치 계산
    Vec2 vPos = GetPos();
    if (m_bHMG)
    {
        if (m_tVRT_Dir == DIR::UP)
        {
            if (m_tHRZ_Dir == DIR::LEFT)
                vPos = Vec2(vPos.x - 10, vPos.y - 180);
            else if (m_tHRZ_Dir == DIR::RIGHT)
                vPos = Vec2(vPos.x - 5, vPos.y - 180);
        }
        else if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow != UPDOWN::BOW)
            vPos = Vec2(vPos.x, vPos.y + 100);
        else if (m_tHRZ_Dir == DIR::LEFT)
            vPos = Vec2(vPos.x - 95, vPos.y + 30);
        else if (m_tHRZ_Dir == DIR::RIGHT)
            vPos = Vec2(vPos.x + 95, vPos.y + 30);

        if (m_tJumpBow == UPDOWN::BOW)
        {
            vPos = vPos - Vec2(0, GetScale().y * 0.5f);
            GetCollider()->SetScale(Vec2(80.f, 60.f));
            GetCollider()->SetOffsetPos(Vec2(0.f, 30.f));
        }
        else
        {
            vPos = vPos - Vec2(0, GetScale().y * 0.5f);
            GetCollider()->SetScale(Vec2(80.f, 120.f));
            GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
        }
    }
    else
    {
        if (m_tVRT_Dir == DIR::UP)
        {
            if (m_tHRZ_Dir == DIR::LEFT)
                vPos = Vec2(vPos.x - 10, vPos.y - 100);
            else if (m_tHRZ_Dir == DIR::RIGHT)
                vPos = Vec2(vPos.x - 5, vPos.y - 100);
        }
        else if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow != UPDOWN::BOW)
            vPos = Vec2(vPos.x, vPos.y + 100);
        else if (m_tHRZ_Dir == DIR::LEFT)
            vPos = Vec2(vPos.x - 100, vPos.y + 5);
        else if (m_tHRZ_Dir == DIR::RIGHT)
            vPos = Vec2(vPos.x + 100, vPos.y + 5);

        if (m_tJumpBow == UPDOWN::BOW)
        {
            vPos = vPos - Vec2(0, GetScale().y * 0.3f);
            GetCollider()->SetScale(Vec2(80.f, 60.f));
            GetCollider()->SetOffsetPos(Vec2(0.f, 30.f));
        }
        else
        {
            vPos = vPos - Vec2(0, GetScale().y * 0.5f);
            GetCollider()->SetScale(Vec2(80.f, 120.f));
            GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
        }
    }

    if (m_fInvincibleTime != 0.f)
    {
        m_fInvincibleTime -= DT;
        m_fInvincibleTime = max(m_fInvincibleTime, 0.f);
    }

    RenewalAttack(vPos);

    if (m_bDead)
    {
        if (m_fDeadTime == 0.f)
        {
            PlayAnim(L"DEAD_SLIME.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
        }

        m_fDeadTime += DT;

        if (m_fDeadTime > 2.7f)
        {
            m_bDead = false;
            --m_iLiftCount;
            SetPos(Vec2(100.f, 500.f));
            PlayAnim(L"SUMON.anim", SETPRINT::NORMAL, false, true);
            m_fInvincibleTime = 3.f;
            m_fDeadTime       = 0.f;
            m_iBulletCount    = 0;
            m_iBombCount      = 10;
            m_bHMG            = false;
        }
    }
}

void CPlayer::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CPlayer::BeginOverlap(CCollider* _pOther)
{
    if (m_fInvincibleTime == 0.f && _pOther->GetOwner()->GetLayer() == LAYER::MONSTER_PROJECTILE && !m_bGod)
        m_bDead = true;
}

void CPlayer::OnOverlap(CCollider* _pOther)
{
}

void CPlayer::EndOverlap(CCollider* _pOther)
{
}

void CPlayer::RenewalPos()
{
    if (IsPressed(KEY::LEFT) && GetPos().x - GetScale().x / 2 > 0.f)
    {
        if (m_tJumpBow == UPDOWN::BOW)
        {
            if (m_tUpperState == STATE::ATTACK)
                return;
            SetPos(Vec2(GetPos().x - m_fSpeed * DT / 3, GetPos().y));
        }
        else
        {
            SetPos(Vec2(GetPos().x - m_fSpeed * DT, GetPos().y));
        }
    }


    if (GetPos().x < m_vResolution.x * m_fCameraRatio)
    {
        CCamera::GetInst()->SetCameraStop(true);
        if (IsPressed(KEY::RIGHT))
        {
            if (m_tJumpBow == UPDOWN::BOW)
            {
                if (m_tUpperState == STATE::ATTACK || m_tUpperState == STATE::BOMB)
                    return;
                SetPos(Vec2(GetPos().x + m_fSpeed * DT / 3, GetPos().y));
            }
            else
            {
                SetPos(Vec2(GetPos().x + m_fSpeed * DT, GetPos().y));
            }
        }
    }
    else
    {
        if (CCamera::GetInst()->GetRealPos(GetPos()).x + m_vResolution.x / 2 < 6322)
        {
            if (m_tJumpBow == UPDOWN::BOW)
            {
                if (m_tUpperState == STATE::ATTACK || m_tUpperState == STATE::BOMB)
                {
                    if (m_tUpperState == STATE::ATTACK)
                        CCamera::GetInst()->SetCameraStop(true);
                    return;
                }
                CCamera::GetInst()->SetCameraSpeed(m_fSpeed / 9);
            }
            else
            {
                CCamera::GetInst()->SetCameraSpeed(m_fSpeed / 3);
            }

            CCamera::GetInst()->SetCameraStop(false);
            const Vec2 vLook = CCamera::GetInst()->GetLook();
            CCamera::GetInst()->SetLook(Vec2(vLook.x + CCamera::GetInst()->GetCameraSpeed() * DT, vLook.y));
            SetPos(Vec2(GetPos().x - 10 * DT, GetPos().y));
        }
        else if (CCamera::GetInst()->GetRealPos(GetPos()).x + m_vResolution.x / 2 > 6321)
        {
            CCamera::GetInst()->SetCameraStop(true);
            if (IsPressed(KEY::RIGHT))
            {
                if (m_tJumpBow == UPDOWN::BOW)
                {
                    if (m_tUpperState == STATE::ATTACK || m_tUpperState == STATE::BOMB)
                        return;
                    SetPos(Vec2(GetPos().x + m_fSpeed * DT / 3, GetPos().y));
                }
                else
                {
                    SetPos(Vec2(GetPos().x + m_fSpeed * DT, GetPos().y));
                }
            }
        }
    }

    if (IsTap(KEY::X) && GetRigidbody()->IsGround())
    {
        GetRigidbody()->AddVelocity(Vec2(0.f, -800.f));
    }
}

void CPlayer::RenewalDir()
{
    const Vec2 Diff = m_vPrevPos - GetPos();
    if (Diff.x > 0.3f)
    {
        SetHorizonDir(DIR::LEFT);
        m_tUpperState = STATE::RUN;
        m_tLowerState = STATE::RUN;
    }
    else if (Diff.x < -0.3f) //|| GetPos().x > m_vResolution.x * m_fCameraRatio)
    {
        SetHorizonDir(DIR::RIGHT);
        m_tUpperState = STATE::RUN;
        m_tLowerState = STATE::RUN;
    }
    else if (Diff.x == 0)
    {
        m_tUpperState = STATE::IDLE;
        m_tLowerState = STATE::IDLE;
    }


    if (IsPressed(KEY::DOWN))
    {
        if (m_tJumpBow == UPDOWN::JUMP)
        {
            SetVerticalDir(DIR::DOWN);
            m_tDir = DIR::DOWN;
        }
        else
        {
            m_tVRT_Dir = DIR::DOWN;
            m_tJumpBow = UPDOWN::BOW;
        }
    }
    else if (IsRelease(KEY::DOWN))
    {
        m_tVRT_Dir = DIR::NONE;
        SetDir(m_tHRZ_Dir);
        m_tDir = DIR::NONE;
        if (m_tJumpBow != UPDOWN::JUMP)
            m_tJumpBow = UPDOWN::IDLE;
    }

    if (IsPressed(KEY::UP))
    {
        SetVerticalDir(DIR::UP);
        m_tDir = DIR::DOWN;
    }
    else if (IsRelease(KEY::UP))
    {
        m_tVRT_Dir = DIR::NONE;
        SetDir(m_tHRZ_Dir);
        m_tDir = DIR::NONE;
    }

    if (GetRigidbody()->IsGround())
    {
        if (IsPressed(KEY::DOWN))
            m_tJumpBow = UPDOWN::BOW;
        else
        {
            if (m_tHRZ_Dir != DIR::UP)
                m_tJumpBow = UPDOWN::IDLE;
        }
    }

    if (GetPixelCollider()->IsJump())
        m_tJumpBow = UPDOWN::JUMP;

    // 공격
    if (IsTap(KEY::C))
    {
        m_fAttackDelay = 0.f;
        if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow == UPDOWN::IDLE)
            return;

        m_tUpperState = STATE::ATTACK;
        m_bAttackOn   = true;
        PlayUpperAnim();
    }

    if (IsPressed(KEY::C))
    {
        m_fAttackTime += DT;
        if (m_bCQC)
        {
            if (m_fAttackTime > 0.6f)
            {
                if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow == UPDOWN::IDLE)
                    return;
                m_tUpperState = STATE::ATTACK;
                m_bAttackOn   = true;
                PlayUpperAnim();
                m_fAttackTime = 0.f;
            }
        }
        else
        {
            if (m_fAttackTime > 0.15f)
            {
                if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow == UPDOWN::IDLE)
                    return;
                m_tUpperState = STATE::ATTACK;
                m_bAttackOn   = true;
                PlayUpperAnim();
                m_fAttackTime = 0.f;
            }
        }
    }

    if (IsTap(KEY::Z))
    {
        --m_iBombCount;
        m_fBombDelay = 0.f;
        if (m_iBombCount < 1)
            m_iBombCount = 0;
        m_tUpperState = STATE::BOMB;
        m_bBombOn     = true;
        PlayUpperAnim();
    }
}

void CPlayer::PlayUpperAnim()
{
    if (m_bDead == false)
    {
        if (m_tHRZ_Dir == DIR::RIGHT)
            m_bPlayerDir = true;

        else if (m_tHRZ_Dir == DIR::LEFT)
            m_bPlayerDir = false;

        if (m_bHMG)
        {
            if (m_tVRT_Dir == DIR::UP)
            {
                if (m_tUpperState == STATE::ATTACK)
                    PlayAnim(L"HMG_SHOT_UP_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                else if (m_tUpperState == STATE::RUN || m_tUpperState == STATE::IDLE)
                    PlayAnim(L"HMG_LOOKUP_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                else if (m_tUpperState == STATE::BOMB)
                    PlayAnim(L"HMG_BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
            }

            else if (m_tVRT_Dir == DIR::DOWN)
            {
                if (m_tUpperState == STATE::ATTACK)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                    {
                        PlayAnim(L"HMG_SHOT_DOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    }
                    else if (m_tJumpBow == UPDOWN::BOW)
                    {
                        if (m_bCQC)
                            PlayAnim(L"HMG_BOW_CQC.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                        else
                            PlayAnim(L"HMG_BOW_SHOT.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                    }
                }
                else if (m_tUpperState == STATE::RUN)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"HMG_LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_BOW_RUN.anim", SETPRINT::NORMAL, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::IDLE)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"HMG_LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_BOW_IDLE.anim", SETPRINT::NORMAL, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::BOMB)
                {
                    if (m_tJumpBow == UPDOWN::BOW)
                        PlayAnim(L"HMG_BOW_BOMB.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
            }
            else
            {
                if (m_tUpperState == STATE::ATTACK)
                {
                    if (m_bCQC)
                        PlayAnim(L"HMG_CQC_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_SHOT_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::RUN)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"HMG_JUMP_RUN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_RUN_UPPER.anim", SETPRINT::UPPER, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::IDLE)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"HMG_JUMP_IDLE_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"HMG_IDLE_UPPER.anim", SETPRINT::UPPER, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::BOMB)
                {
                    PlayAnim(L"HMG_BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
            }
        }

        // =============================================

        else
        {
            if (m_tVRT_Dir == DIR::UP)
            {
                if (m_tUpperState == STATE::ATTACK)
                    PlayAnim(L"SHOT_UP_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                else if (m_tUpperState == STATE::RUN || m_tUpperState == STATE::IDLE)
                    PlayAnim(L"LOOKUP_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                else if (m_tUpperState == STATE::BOMB)
                    PlayAnim(L"BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
            }

            else if (m_tVRT_Dir == DIR::DOWN)
            {
                if (m_tUpperState == STATE::ATTACK)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                    {
                        PlayAnim(L"SHOT_DOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    }
                    else if (m_tJumpBow == UPDOWN::BOW)
                    {
                        if (m_bCQC)
                            PlayAnim(L"BOW_CQC.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                        else
                            PlayAnim(L"BOW_SHOT.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                    }
                }
                else if (m_tUpperState == STATE::RUN)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"BOW_RUN.anim", SETPRINT::NORMAL, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::IDLE)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"BOW_IDLE.anim", SETPRINT::NORMAL, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::BOMB)
                {
                    if (m_tJumpBow == UPDOWN::BOW)
                        PlayAnim(L"BOW_BOMB.anim", SETPRINT::NORMAL, false, m_bPlayerDir);
                    else
                        PlayAnim(L"BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
            }
            else
            {
                if (m_tUpperState == STATE::ATTACK)
                {
                    if (m_bCQC)
                        PlayAnim(L"CQC_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"SHOT_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::RUN)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"JUMP_RUN_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"RUN_UPPER.anim", SETPRINT::UPPER, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::IDLE)
                {
                    if (m_tJumpBow == UPDOWN::JUMP)
                        PlayAnim(L"JUMP_IDLE_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                    else
                        PlayAnim(L"IDLE_UPPER.anim", SETPRINT::UPPER, true, m_bPlayerDir);
                }
                else if (m_tUpperState == STATE::BOMB)
                {
                    PlayAnim(L"BOMB_UPPER.anim", SETPRINT::UPPER, false, m_bPlayerDir);
                }
            }
        }
    }
}

void CPlayer::PlayLowerAnim()
{
    if (m_bDead == false)
    {
        if (m_tHRZ_Dir == DIR::RIGHT)
            m_bPlayerDir = true;

        else if (m_tHRZ_Dir == DIR::LEFT)
            m_bPlayerDir = false;

        if (m_tLowerState == STATE::RUN)
        {
            if (m_tJumpBow == UPDOWN::JUMP)
            {
                PlayAnim(L"JUMP_RUN_LOWER.anim", SETPRINT::LOWER, false, m_bPlayerDir);
            }
            else
            {
                PlayAnim(L"RUN_LOWER.anim", SETPRINT::LOWER, true, m_bPlayerDir);
            }
        }
        else if (m_tLowerState == STATE::IDLE)
        {
            if (m_tJumpBow == UPDOWN::JUMP)
            {
                PlayAnim(L"JUMP_IDLE_LOWER.anim", SETPRINT::LOWER, false, m_bPlayerDir);
            }
            else
            {
                PlayAnim(L"IDLE_LOWER.anim", SETPRINT::LOWER, true, m_bPlayerDir);
            }
        }
    }
}

void CPlayer::PlayAnim(const wstring& _strName, const SETPRINT _tUpDownNormal, const bool _bRepeat, const bool _bDirRight)
{
    // 상하체 구분해서 재생하기
    if (_tUpDownNormal == SETPRINT::UPPER)
    {
        GetAnimator()->Play(_strName, _bRepeat);
        GetAnimator()->GetAnimation()->SetUpper();
        if (_bDirRight)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
    }
    else if (_tUpDownNormal == SETPRINT::LOWER)
    {
        GetAnimator2()->Play(_strName, _bRepeat);
        GetAnimator2()->GetAnimation()->SetLower();
        if (_bDirRight)
            GetAnimator2()->GetAnimation()->SetPlayRight();
        else
            GetAnimator2()->GetAnimation()->SetPlayLeft();
    }
    else if (_tUpDownNormal == SETPRINT::NORMAL)
    {
        GetAnimator()->Play(_strName, _bRepeat);
        GetAnimator2()->Play(L"NOTHING_LOWER.anim", false);
        if (_bDirRight)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
    }
}


void CPlayer::LoadAnim() const
{
    GetAnimator()->LoadAnimation(L"Player//IDLE_GUM.anim");
    GetAnimator()->LoadAnimation(L"Player//IDLE_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//LOOKUP_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//LOOKDOWN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//RUN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//RUNSTART_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//JUMP_IDLE_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//JUMP_RUN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//SHOT_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//SHOT_UP_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//SHOT_DOWN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//CQC_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//BOMB_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_CQC_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_IDLE_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_JUMP_IDLE_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_JUMP_RUN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_LOOKDOWN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_LOOKUP_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_RUN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_SHOT_DOWN_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_SHOT_UP_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_SHOT_UPPER.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOMB_UPPER.anim");

    // ==================================================================
    GetAnimator2()->LoadAnimation(L"Player//IDLE_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//RUN_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//RUNSTART_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//NOTHING_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//JUMP_IDLE_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//JUMP_RUN_LOWER.anim");

    // ==================================================================
    GetAnimator()->LoadAnimation(L"Player//BOW_IDLE.anim");
    GetAnimator()->LoadAnimation(L"Player//BOW_RUN.anim");
    GetAnimator()->LoadAnimation(L"Player//BOW_START.anim");
    GetAnimator()->LoadAnimation(L"Player//BOW_CQC.anim");
    GetAnimator()->LoadAnimation(L"Player//BOW_SHOT.anim");
    GetAnimator()->LoadAnimation(L"Player//BOW_BOMB.anim");
    GetAnimator()->LoadAnimation(L"Player//SUMON.anim");
    GetAnimator()->LoadAnimation(L"Player//DEAD_GROUND.anim");
    GetAnimator()->LoadAnimation(L"Player//DEAD_NORMAL.anim");
    GetAnimator()->LoadAnimation(L"Player//DEAD_SLIME.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOW_BOMB.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOW_CQC.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOW_IDLE.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOW_SHOT.anim");
    GetAnimator()->LoadAnimation(L"Player//HMG_BOW_RUN.anim");
    GetAnimator()->LoadAnimation(L"Player//VICTORY.anim");
}

void CPlayer::RenewalAttack(const Vec2& vPos)
{
    if (m_bDead == false)
    {
        if (IsTap(KEY::C))
        {
            if (m_bCQC)
            {
            }
            else if (m_bHMG)
            {
                CHandgun* pBullet = new CHandgun;
                pBullet->SetPos(Vec2(vPos.x, vPos.y));
                pBullet->SetDir(m_fPlayerDir);
                if (m_tDir != DIR::NONE)
                    pBullet->SetVRT();
                Instantiate(pBullet, vPos, LAYER::PLAYER_PROJECTILE);
                --m_iBulletCount;
            }
            else
            {
                CHandgun* pBullet = new CHandgun;
                pBullet->SetPos(Vec2(vPos.x, vPos.y));
                pBullet->SetDir(m_fPlayerDir);
                if (m_tDir != DIR::NONE)
                    pBullet->SetVRT();
                Instantiate(pBullet, vPos, LAYER::PLAYER_PROJECTILE);
            }
        }

        if (IsPressed(KEY::C))
        {
            if (m_bCQC && m_fCQCDelay > 0.25f)
            {
                CPlayerCQC* pCQC = new CPlayerCQC;
                pCQC->SetPos(Vec2(vPos.x, vPos.y));
                Instantiate(pCQC, vPos, LAYER::CQC);
                m_fCQCDelay = 0.f;
            }
            else if (m_bHMG && m_fAttackDelay > 0.05f)
            {
                CHandgun* pBullet = new CHandgun;
                pBullet->SetPos(Vec2(vPos.x, vPos.y));
                pBullet->SetDir(m_fPlayerDir);
                if (m_tDir != DIR::NONE)
                    pBullet->SetVRT();
                Instantiate(pBullet, vPos, LAYER::PLAYER_PROJECTILE);
                m_fAttackDelay = 0.f;
                --m_iBulletCount;
            }
            else
            {
                if (m_fAttackDelay > 0.15)
                {
                    CHandgun* pBullet = new CHandgun;
                    pBullet->SetPos(Vec2(vPos.x, vPos.y));
                    pBullet->SetDir(m_fPlayerDir);
                    if (m_tDir != DIR::NONE)
                        pBullet->SetVRT();
                    Instantiate(pBullet, vPos, LAYER::PLAYER_PROJECTILE);
                    m_fAttackDelay = 0.f;
                }
            }
        }
        else if (IsTap(KEY::Z))
        {
            if (m_bBombOn && m_iBombCount > 0)
            {
                CBomb* pBomb = new CBomb;
                pBomb->SetPos(vPos);
                if (m_tHRZ_Dir == DIR::LEFT)
                    pBomb->SetDir(false);
                else
                    pBomb->SetDir(true);
                Instantiate(pBomb, vPos, LAYER::PLAYER_FIREPROJECTILE);
            }
        }

        if (m_bAttackOn)
        {
            if (m_tHRZ_Dir != m_tShiftHRZDir || m_tJumpBow != m_tShiftJumpBow)
            {
                m_tUpperState = STATE::RUN;
                PlayUpperAnim();
                PlayLowerAnim();
                return;
            }

            if (m_bCQC)
            {
                m_fCQCDelay    += DT;
                m_fAttackDelay += DT;
                m_tUpperState  = STATE::ATTACK;
                if (m_fAttackDelay > 0.6f) // 공격 딜레이가 일정 시간 지나면 원래 상태로
                {
                    m_tUpperState = STATE::IDLE;
                    PlayUpperAnim();
                    m_fAttackDelay = 0.f;
                    m_fCQCDelay    = 0.f;
                    m_bAttackOn    = false;
                }
                if (m_bHMG && m_tJumpBow == UPDOWN::BOW && m_fAttackDelay > 0.4f)
                {
                    m_tUpperState = STATE::IDLE;
                    PlayUpperAnim();
                    m_fAttackDelay = 0.f;
                    m_bAttackOn    = false;
                }
            }
            else if (m_bHMG)
            {
                if (m_iBulletCount < 1)
                {
                    m_tUpperState  = STATE::IDLE;
                    m_fAttackDelay = 0.f;
                    m_bAttackOn    = false;
                    m_bHMG         = false;
                    return;
                }
                m_fAttackDelay += DT;
                m_tUpperState  = STATE::ATTACK;
                if (m_fAttackDelay > 0.28f) // 공격 딜레이가 일정 시간 지나면 원래 상태로
                {
                    m_tUpperState = STATE::IDLE;
                    PlayUpperAnim();
                    m_fAttackDelay = 0.f;
                    m_bAttackOn    = false;
                }
            }
            else
            {
                m_fAttackDelay += DT;
                m_tUpperState  = STATE::ATTACK;
                if (m_fAttackDelay > 0.52f) // 공격 딜레이가 일정 시간 지나면 원래 상태로
                {
                    m_tUpperState = STATE::IDLE;
                    PlayUpperAnim();
                    m_fAttackDelay = 0.f;
                    m_bAttackOn    = false;
                }
            }
        }

        if (m_bBombOn)
        {
            if (m_tHRZ_Dir != m_tShiftHRZDir || m_tJumpBow != m_tShiftJumpBow)
            {
                m_tUpperState = STATE::RUN;
                m_bBombOn     = false;
                m_fBombDelay  = 0.f;
                PlayUpperAnim();
                PlayLowerAnim();
                return;
            }
            m_fBombDelay  += DT;
            m_tUpperState = STATE::BOMB;
            if (m_fBombDelay > 0.5f) // 폭탄 딜레이가 일정 시간 지나면 원래 상태로
            {
                m_tUpperState = STATE::IDLE;
                PlayUpperAnim();
                m_fBombDelay = 0.f;
                m_bBombOn    = false;
            }
        }
    }
}
