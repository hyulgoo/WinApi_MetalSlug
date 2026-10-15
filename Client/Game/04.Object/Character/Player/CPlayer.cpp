#include "pch.h"
#include "Game\04.Object\Character\Player\CPlayer.h"

#include <algorithm>

#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\04.Object\Bullet\Player\CHandgun.h"
#include "Game\04.Object\Bullet\Player\CPlayerCQC.h"
#include "Game\04.Object\Bullet\Player\CBomb.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\05.Component\Rigidbody\CRigidbody.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\04.Object\Bullet\Player\CCQCArea.h"

#include "Game\01.Engine\CEngine.h"

namespace
{
    // 카메라가 멈추는 스테이지 끝 (아래 fStageEnd 식의 값. 예전 코드의 "< 6322 / > 6321" 두 조건을 하나로)
    constexpr float STAGE_END_X = 6322.f;

    // 총구 자리: 몸 기준점에서 총알 그림의 왼쪽 위까지. 무기마다 다르다
    struct tMuzzle
    {
        Vec2  vUpLeft;     // 위를 볼 때 (왼쪽·오른쪽을 보고 있던 쪽)
        Vec2  vUpRight;
        Vec2  vDown;       // 공중에서 아래를 볼 때
        Vec2  vLeft;       // 옆
        Vec2  vRight;
        float fBowRaise;   // 앉았을 때 몸 높이의 이만큼 위 (서 있으면 0.5)
    };
    constexpr tMuzzle MUZZLE_PISTOL = { Vec2(-10.f, -100.f), Vec2(-5.f, -100.f), Vec2(0.f, 100.f), Vec2(-100.f, 5.f), Vec2(100.f, 5.f), 0.3f };
    constexpr tMuzzle MUZZLE_HMG    = { Vec2(-10.f, -180.f), Vec2(-5.f, -180.f), Vec2(0.f, 100.f), Vec2(-95.f, 30.f), Vec2(95.f, 30.f), 0.5f };

    // 카메라를 미는 동안 플레이어를 기준선 쪽으로 되돌리는 속도 (px/s)
    constexpr float CAMERA_PUSHBACK_SPEED = 10.f;

    // 이 속도(px/s)보다 빠르게 움직였을 때만 이동으로 보고 방향을 바꾼다.
    // 되돌림 속도보다 크고 앉아서 걷는 속도(m_fSpeed / 3)보다 작아야 한다.
    constexpr float MOVE_DETECT_SPEED = 50.f;

    using STATE  = CPlayer::STATE;
    using UPDOWN = CPlayer::UPDOWN;

    // 상체 애니메이션 하나 (권총 이름. HMG를 들면 앞에 "HMG_"를 붙인 같은 이름의 애니메이션이 있다)
    struct tUpperAnim
    {
        const wchar_t* szName  = nullptr;   // nullptr이면 바꾸지 않는다
        SETPRINT       ePrint  = SETPRINT::UPPER;
        bool           bRepeat = false;
    };

    // 상체 애니메이션 선택 표: 세로 방향(위·아래·없음) x 상체 상태 x 점프·앉기 x 칼 거리
    // 예전에는 같은 if-else가 권총용과 HMG용으로 두 벌(173줄) 있었다
    tUpperAnim SelectUpperAnim(const DIR _eVRT, const STATE _eUpper, const UPDOWN _eJumpBow, const bool _bCQC)
    {
        const bool bJump = UPDOWN::JUMP == _eJumpBow;
        const bool bBow  = UPDOWN::BOW == _eJumpBow;

        if (DIR::UP == _eVRT)
        {
            switch (_eUpper)
            {
            case STATE::ATTACK: return { L"SHOT_UP_UPPER.anim", SETPRINT::UPPER, false };
            case STATE::RUN:
            case STATE::IDLE:   return { L"LOOKUP_UPPER.anim", SETPRINT::UPPER, false };
            case STATE::BOMB:   return { L"BOMB_UPPER.anim", SETPRINT::UPPER, false };
            default:            return {};
            }
        }

        if (DIR::DOWN == _eVRT)
        {
            switch (_eUpper)
            {
            case STATE::ATTACK:
                if (bJump)
                    return { L"SHOT_DOWN_UPPER.anim", SETPRINT::UPPER, false };
                if (bBow)
                    return { _bCQC ? L"BOW_CQC.anim" : L"BOW_SHOT.anim", SETPRINT::NORMAL, false };
                return {};
            case STATE::RUN:
                return bJump ? tUpperAnim{ L"LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false } : tUpperAnim{ L"BOW_RUN.anim", SETPRINT::NORMAL, true };
            case STATE::IDLE:
                return bJump ? tUpperAnim{ L"LOOKDOWN_UPPER.anim", SETPRINT::UPPER, false } : tUpperAnim{ L"BOW_IDLE.anim", SETPRINT::NORMAL, true };
            case STATE::BOMB:
                return bBow ? tUpperAnim{ L"BOW_BOMB.anim", SETPRINT::NORMAL, false } : tUpperAnim{ L"BOMB_UPPER.anim", SETPRINT::UPPER, false };
            default:
                return {};
            }
        }

        switch (_eUpper)
        {
        case STATE::ATTACK: return { _bCQC ? L"CQC_UPPER.anim" : L"SHOT_UPPER.anim", SETPRINT::UPPER, false };
        case STATE::RUN:    return bJump ? tUpperAnim{ L"JUMP_RUN_UPPER.anim", SETPRINT::UPPER, false } : tUpperAnim{ L"RUN_UPPER.anim", SETPRINT::UPPER, true };
        case STATE::IDLE:   return bJump ? tUpperAnim{ L"JUMP_IDLE_UPPER.anim", SETPRINT::UPPER, false } : tUpperAnim{ L"IDLE_UPPER.anim", SETPRINT::UPPER, true };
        case STATE::BOMB:   return { L"BOMB_UPPER.anim", SETPRINT::UPPER, false };
        default:            return {};
        }
    }
}

CPlayer::CPlayer()
{
    CreateCollider();
    CreateAnimator();
    CreateAnimator2();
    CreateRigidbody();
    CreatePixelCollider();

    SetScale(Vec2(80.f, 120.f));

    m_vResolution = CEngine::GetInst()->GetResolution();

    LoadAnim();

    GetRigidbody()->SetVelocityLimit(200.f);
    GetRigidbody()->SetFriction(500.f);
    GetRigidbody()->SetGravity(true);
    GetRigidbody()->SetGravityAccel(1000.f);

    m_pCQC = new CCQCArea;
    m_pCQC->SetPos(GetPos());
    Instantiate(m_pCQC, GetPos(), LAYER::CQCAREA);   // 칼 범위 감지용. 칼 판정(CPlayerCQC)은 CQC
}

CPlayer::~CPlayer()
{
}

void CPlayer::tick()
{
    CObj::tick();
    ReadInput();

    if (m_bSummon)
    {
        if (m_bStart)
        {
            PlayAnim(L"SUMMON.anim", SETPRINT::NORMAL, false, true);
            m_bStart = false;
        }
        m_fSummonTime += DT;
        if (m_fSummonTime > 0.43f)
        {
            m_bSummon = false;
            PlayLowerAnim();
            PlayUpperAnim();
        }
        return;
    }

    if constexpr (DEBUG_KEYS)
    {
        if (m_tInput.bDebugVictory)
            PlayAnim(L"VICTORY.anim", SETPRINT::NORMAL, true, true);
        if (m_tInput.bDebugGod)
            ToggleGod();
    }

    // 이번 tick을 시작할 때의 상태 (바뀌면 애니메이션을 다시 고른다)
    m_tShiftUpperState = m_tUpperState;
    m_tShiftLowerState = m_tLowerState;
    m_vPrevPos         = GetScreenPos();   // 움직임 판정은 화면 기준 (카메라를 미는 동안은 화면에서 거의 서 있다)
    m_tShiftHRZDir     = m_tHRZ_Dir;
    m_tShiftVRTDir     = m_tVRT_Dir;
    m_tShiftJumpBow    = m_tJumpBow;
    if (!m_bDead)
    {
        UpdateMovement();
        UpdateBodyState();
    }

    // 칼 범위 감지 영역은 보고 있는 쪽 앞에 둔다
    m_pCQC->SetPos(GetPos() + Vec2(DIR::LEFT == m_tHRZ_Dir ? -50.f : 50.f, 0.f));
    m_bCQC = m_pCQC->GetCQC();

    UpdateAnimation();

    const Vec2 vMuzzle = GetMuzzlePos();
    UpdateCollider();

    if (m_fInvincibleTime != 0.f)
    {
        m_fInvincibleTime -= DT;
        m_fInvincibleTime = max(m_fInvincibleTime, 0.f);
    }

    UpdateWeapon(vMuzzle);
    UpdateDeath();
}

void CPlayer::ReadInput()
{
    m_tInput              = {};
    m_tInput.bLeft        = IsPressed(KEY::LEFT);
    m_tInput.bRight       = IsPressed(KEY::RIGHT);
    m_tInput.bUp          = IsPressed(KEY::UP);
    m_tInput.bUpRelease   = IsRelease(KEY::UP);
    m_tInput.bDown        = IsPressed(KEY::DOWN);
    m_tInput.bDownRelease = IsRelease(KEY::DOWN);
    m_tInput.bJump        = IsTap(KEY::X);
    m_tInput.bFireTap     = IsTap(KEY::C);
    m_tInput.bFireHold    = IsPressed(KEY::C);
    m_tInput.bBomb        = IsTap(KEY::Z);

    if constexpr (DEBUG_KEYS)
    {
        m_tInput.bDebugVictory = IsTap(KEY::F);
        m_tInput.bDebugGod     = IsTap(KEY::V);
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

void CPlayer::UpdateMovement()
{
    MoveHorizontal();

    // 점프는 좌우 이동과 따로 받는다 (예전에는 앉아서 쏘는 중에 방향키를 누르면 이동을 막는 중간 return이
    // 점프 입력까지 건너뛰었다)
    if (m_tInput.bJump && GetRigidbody()->IsGround())
        GetRigidbody()->AddVelocity(Vec2(0.f, -800.f));
}

void CPlayer::MoveHorizontal()
{
    // 앉아서 쏘거나(왼쪽) 쏘거나 던지는(오른쪽) 동안은 걷지 않는다
    // 화면 끝·카메라를 미는 선은 화면 위치로 판정하고, 이동은 월드 위치로 한다
    if (m_tInput.bLeft && GetScreenPos().x - GetScale().x / 2 > 0.f)
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

    if (GetScreenPos().x < m_vResolution.x * m_fCameraRatio)
    {
        if (m_tInput.bRight)
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
        return;
    }

    // 스테이지 끝 판정은 예전 식 그대로 둔다: 화면 x + 카메라 왼쪽 위의 배경 텍스처 x + 화면 너비 절반.
    // 단위가 섞여 있지만 카메라가 멈추는 자리를 바꾸지 않기 위해서다
    const float fStageEnd = GetScreenPos().x + CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)).x / BG_MAGNI + m_vResolution.x / 2;
    if (fStageEnd < STAGE_END_X)
    {
        // 카메라를 미는 속도(배경 텍스처 px/s). 앉으면 더 느리다
        float fScrollSpeed = m_fSpeed / 3;
        if (m_tJumpBow == UPDOWN::BOW)
        {
            if (m_tUpperState == STATE::ATTACK || m_tUpperState == STATE::BOMB)
                return;
            fScrollSpeed = m_fSpeed / 9;
        }

        // 카메라를 밀고, 플레이어도 카메라와 같이 나아가되 화면에서는 천천히 뒤로 밀린다
        const float fScroll = fScrollSpeed * BG_MAGNI * DT;
        const Vec2  vLook   = CCamera::GetInst()->GetLook();
        CCamera::GetInst()->SetLook(Vec2(vLook.x + fScroll, vLook.y));
        SetPos(Vec2(GetPos().x + fScroll - CAMERA_PUSHBACK_SPEED * DT, GetPos().y));
    }
    else
    {
        // 카메라가 스테이지 끝에 고정되면 화면 오른쪽 밖으로 나가지 못하게 막는다.
        if (m_tInput.bRight && GetScreenPos().x + GetScale().x / 2 < m_vResolution.x)
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

void CPlayer::FireBullet(const Vec2& _vMuzzle)
{
    // _vMuzzle은 총알 그림의 왼쪽 위가 오도록 맞춰 둔 총구 자리다. 총알 위치는 그림의 중심이라 반 크기를 더한다
    CHandgun* pBullet = new CHandgun;
    pBullet->SetDir(m_fPlayerDir);
    if (m_tDir != DIR::NONE)
        pBullet->SetVRT();
    Instantiate(pBullet, _vMuzzle + pBullet->GetSpriteSize() / 2.f, LAYER::PLAYER_PROJECTILE);
}

void CPlayer::UpdateBodyState()
{
    // 좌우: 화면에서 실제로 움직인 쪽으로 몸을 돌리고 달린다
    const Vec2 Diff = m_vPrevPos - GetScreenPos();

    // 고정 거리(0.3px)로 판정하면 프레임 시간이 30ms를 넘을 때 카메라 되돌림(10px/s)이
    // 왼쪽 이동으로 판정되어 오른쪽으로 달리던 캐릭터가 뒤집힌다. 프레임 시간에 비례한 기준을 쓴다.
    const float fMoveThreshold = MOVE_DETECT_SPEED * DT;
    if (Diff.x > fMoveThreshold)
    {
        SetHorizonDir(DIR::LEFT);
        m_tUpperState = STATE::RUN;
        m_tLowerState = STATE::RUN;
    }
    else if (Diff.x < -fMoveThreshold)
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

    // 아래: 공중이면 아래를 보고, 땅이면 앉는다
    if (m_tInput.bDown)
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
    else if (m_tInput.bDownRelease)
    {
        m_tVRT_Dir = DIR::NONE;
        SetDir(m_tHRZ_Dir);
        m_tDir = DIR::NONE;
        if (m_tJumpBow != UPDOWN::JUMP)
            m_tJumpBow = UPDOWN::IDLE;
    }

    // 위: 위를 본다
    if (m_tInput.bUp)
    {
        SetVerticalDir(DIR::UP);
        m_tDir = DIR::DOWN;
    }
    else if (m_tInput.bUpRelease)
    {
        m_tVRT_Dir = DIR::NONE;
        SetDir(m_tHRZ_Dir);
        m_tDir = DIR::NONE;
    }

    if (GetRigidbody()->IsGround())
    {
        if (m_tInput.bDown)
            m_tJumpBow = UPDOWN::BOW;
        else if (m_tHRZ_Dir != DIR::UP)
            m_tJumpBow = UPDOWN::IDLE;
    }

    if (GetPixelCollider()->IsAirborne())
        m_tJumpBow = UPDOWN::JUMP;

    // 사격: 누른 순간과, 누르고 있는 동안 일정 간격(칼 거리 0.6초, 아니면 0.15초)으로 상체를 사격 상태로
    if (m_tInput.bFireTap)
    {
        m_fAttackDelay = 0.f;
        if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow == UPDOWN::IDLE)
            return;

        m_tUpperState = STATE::ATTACK;
        m_bAttackOn   = true;
        PlayUpperAnim();
    }

    if (m_tInput.bFireHold)
    {
        m_fAttackTime += DT;
        const float fInterval = m_bCQC ? 0.6f : 0.15f;
        if (m_fAttackTime > fInterval)
        {
            if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow == UPDOWN::IDLE)
                return;
            m_tUpperState = STATE::ATTACK;
            m_bAttackOn   = true;
            PlayUpperAnim();
            m_fAttackTime = 0.f;
        }
    }

    // 폭탄: 남은 개수가 있을 때만 던진다. 개수는 폭탄을 만드는 자리(UpdateWeapon)에서 줄인다
    // (여기서 먼저 줄이면 마지막 폭탄이 무기 쪽 개수 검사에 걸려 나가지 않았다)
    if (m_tInput.bBomb && m_iBombCount > 0)
    {
        m_fBombDelay  = 0.f;
        m_tUpperState = STATE::BOMB;
        m_bBombOn     = true;
        m_bThrowBomb  = true;
        PlayUpperAnim();
    }
}

void CPlayer::UpdateAnimation()
{
    // 이번 tick에 상태(하체·상체, 방향, 점프·앉기) 중 하나라도 바뀌었으면 애니메이션을 다시 고른다
    const bool bCommon = m_tShiftJumpBow != m_tJumpBow || m_tShiftHRZDir != m_tHRZ_Dir || m_tShiftVRTDir != m_tVRT_Dir;
    if (m_tShiftLowerState != m_tLowerState || bCommon)
        PlayLowerAnim();
    if ((m_tShiftUpperState != m_tUpperState || bCommon) && !m_bAttackOn && !m_bBombOn)
        PlayUpperAnim();
}

Vec2 CPlayer::GetMuzzlePos() const
{
    // 위·아래·옆을 볼 때 무기마다 총구 자리가 다르다 (총알 그림의 왼쪽 위가 오는 자리)
    const tMuzzle& tGun = m_bHMG ? MUZZLE_HMG : MUZZLE_PISTOL;

    Vec2 vPos = GetPos();
    if (m_tVRT_Dir == DIR::UP)
    {
        if (m_tHRZ_Dir == DIR::LEFT)
            vPos += tGun.vUpLeft;
        else if (m_tHRZ_Dir == DIR::RIGHT)
            vPos += tGun.vUpRight;
    }
    else if (m_tVRT_Dir == DIR::DOWN && m_tJumpBow != UPDOWN::BOW)
        vPos += tGun.vDown;
    else if (m_tHRZ_Dir == DIR::LEFT)
        vPos += tGun.vLeft;
    else if (m_tHRZ_Dir == DIR::RIGHT)
        vPos += tGun.vRight;

    // 총알은 몸 기준점보다 위에서 나간다. 권총은 앉으면 덜 내려간다
    const float fRaise = (m_tJumpBow == UPDOWN::BOW) ? tGun.fBowRaise : 0.5f;
    return vPos - Vec2(0.f, GetScale().y * fRaise);
}

void CPlayer::UpdateCollider()
{
    // 앉으면 아래 절반만 맞는다
    if (m_tJumpBow == UPDOWN::BOW)
    {
        GetCollider()->SetScale(Vec2(80.f, 60.f));
        GetCollider()->SetOffsetPos(Vec2(0.f, 30.f));
    }
    else
    {
        GetCollider()->SetScale(Vec2(80.f, 120.f));
        GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
    }
}

void CPlayer::UpdateDeath()
{
    if (!m_bDead)
        return;

    if (m_fDeadTime == 0.f)
        PlayAnim(L"DEAD_SLIME.anim", SETPRINT::NORMAL, false, m_bPlayerDir);

    m_fDeadTime += DT;

    if (m_fDeadTime > 2.7f)
    {
        m_bDead = false;
        --m_iLifeCount;
        SetPos(CCamera::GetInst()->GetRealPos(SPAWN_SCREEN_POS));   // 처음 자리에서 다시 나온다
        PlayAnim(L"SUMMON.anim", SETPRINT::NORMAL, false, true);
        m_fInvincibleTime = 3.f;
        m_fDeadTime       = 0.f;
        m_iBulletCount    = 0;
        m_iBombCount      = 10;
        m_bHMG            = false;
    }
}

void CPlayer::PlayUpperAnim()
{
    if (m_bDead)
        return;

    if (m_tHRZ_Dir == DIR::RIGHT)
        m_bPlayerDir = true;
    else if (m_tHRZ_Dir == DIR::LEFT)
        m_bPlayerDir = false;

    const tUpperAnim tAnim = SelectUpperAnim(m_tVRT_Dir, m_tUpperState, m_tJumpBow, m_bCQC);
    if (nullptr == tAnim.szName)
        return;

    const wstring strName = m_bHMG ? wstring(L"HMG_") + tAnim.szName : wstring(tAnim.szName);
    PlayAnim(strName, tAnim.ePrint, tAnim.bRepeat, m_bPlayerDir);
}

void CPlayer::PlayLowerAnim()
{
    if (m_bDead)
        return;

    if (m_tHRZ_Dir == DIR::RIGHT)
        m_bPlayerDir = true;
    else if (m_tHRZ_Dir == DIR::LEFT)
        m_bPlayerDir = false;

    const bool bJump = m_tJumpBow == UPDOWN::JUMP;
    if (m_tLowerState == STATE::RUN)
        PlayAnim(bJump ? L"JUMP_RUN_LOWER.anim" : L"RUN_LOWER.anim", SETPRINT::LOWER, !bJump, m_bPlayerDir);
    else if (m_tLowerState == STATE::IDLE)
        PlayAnim(bJump ? L"JUMP_IDLE_LOWER.anim" : L"IDLE_LOWER.anim", SETPRINT::LOWER, !bJump, m_bPlayerDir);
}

void CPlayer::PlayAnim(const wstring& _strName, const SETPRINT _tUpDownNormal, const bool _bRepeat, const bool _bDirRight)
{
    // 상체는 Animator, 하체는 Animator2. 전신(NORMAL)이면 하체를 비운다
    CAnimator* pAnimator = SETPRINT::LOWER == _tUpDownNormal ? GetAnimator2() : GetAnimator();
    pAnimator->Play(_strName, _bRepeat);
    if (SETPRINT::UPPER == _tUpDownNormal)
        pAnimator->GetAnimation()->SetUpper();
    else if (SETPRINT::LOWER == _tUpDownNormal)
        pAnimator->GetAnimation()->SetLower();
    else
        GetAnimator2()->Play(L"NOTHING_LOWER.anim", false);

    if (_bDirRight)
        pAnimator->GetAnimation()->SetPlayRight();
    else
        pAnimator->GetAnimation()->SetPlayLeft();
}

void CPlayer::LoadAnim() const
{
    // 상체·전신 (권총, HMG). 쓰지 않던 IDLE_GUM·RUNSTART_*·BOW_START·DEAD_GROUND·DEAD_NORMAL은 뺐다
    const wchar_t* arrUpper[] =
    {
        L"IDLE_UPPER", L"LOOKUP_UPPER", L"LOOKDOWN_UPPER", L"RUN_UPPER", L"JUMP_IDLE_UPPER", L"JUMP_RUN_UPPER",
        L"SHOT_UPPER", L"SHOT_UP_UPPER", L"SHOT_DOWN_UPPER", L"CQC_UPPER", L"BOMB_UPPER",
        L"BOW_IDLE", L"BOW_RUN", L"BOW_CQC", L"BOW_SHOT", L"BOW_BOMB",
    };
    for (const wchar_t* szName : arrUpper)
    {
        GetAnimator()->LoadAnimation(wstring(L"Player//") + szName + L".anim");
        GetAnimator()->LoadAnimation(wstring(L"Player//HMG_") + szName + L".anim");
    }
    GetAnimator()->LoadAnimation(L"Player//SUMMON.anim");
    GetAnimator()->LoadAnimation(L"Player//DEAD_SLIME.anim");
    GetAnimator()->LoadAnimation(L"Player//VICTORY.anim");

    // 하체
    GetAnimator2()->LoadAnimation(L"Player//IDLE_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//RUN_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//NOTHING_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//JUMP_IDLE_LOWER.anim");
    GetAnimator2()->LoadAnimation(L"Player//JUMP_RUN_LOWER.anim");
}

void CPlayer::UpdateWeapon(const Vec2& vPos)
{
    if (m_bDead)
        return;

    // 사격 입력: 칼 거리면 칼, 아니면 총 (HMG는 탄을 쓴다)
    if (m_tInput.bFireTap)
    {
        if (m_bCQC)
        {
            // 근접 범위 안이면 누르는 즉시 칼 판정을 만든다 (예전에는 비어 있어 애니메이션만 나왔다)
            Instantiate(new CPlayerCQC, vPos, LAYER::CQC);
            m_fCQCDelay = 0.f;
        }
        else
        {
            FireBullet(vPos);
            if (m_bHMG)
                --m_iBulletCount;
        }
    }

    if (m_tInput.bFireHold)
    {
        if (m_bCQC)
        {
            // 근접 범위 안에서는 칼만 쓴다. 쿨다운 동안 총 분기로 떨어지지 않는다
            if (m_fCQCDelay > 0.25f)
            {
                Instantiate(new CPlayerCQC, vPos, LAYER::CQC);
                m_fCQCDelay = 0.f;
            }
        }
        else if (m_bHMG)
        {
            if (m_fAttackDelay > 0.05f)
            {
                FireBullet(vPos);
                m_fAttackDelay = 0.f;
                --m_iBulletCount;
            }
        }
        else if (m_fAttackDelay > 0.15f)
        {
            FireBullet(vPos);
            m_fAttackDelay = 0.f;
        }
    }

    // 폭탄은 사격과 상관없이 던진다 (예전에는 사격 분기의 else if라 C를 누르고 있으면 개수만 줄었다)
    if (m_bThrowBomb)
    {
        m_bThrowBomb = false;

        CBomb* pBomb = new CBomb;
        pBomb->SetDir(m_tHRZ_Dir != DIR::LEFT);
        Instantiate(pBomb, vPos, LAYER::PLAYER_FIREPROJECTILE);
        --m_iBombCount;
    }

    // 사격 자세 유지 시간: 끝나면 원래 자세로. 도중에 방향·점프가 바뀌면 바로 푼다
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
            if (m_fAttackDelay > 0.6f)
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
            if (m_fAttackDelay > 0.28f)
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
            if (m_fAttackDelay > 0.52f)
            {
                m_tUpperState = STATE::IDLE;
                PlayUpperAnim();
                m_fAttackDelay = 0.f;
                m_bAttackOn    = false;
            }
        }
    }

    // 폭탄 던지는 자세 유지 시간
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
        if (m_fBombDelay > 0.5f)
        {
            m_tUpperState = STATE::IDLE;
            PlayUpperAnim();
            m_fBombDelay = 0.f;
            m_bBombOn    = false;
        }
    }
}
