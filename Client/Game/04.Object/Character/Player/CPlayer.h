#pragma once
#include "Game\04.Object\CObj.h"

class CCQCArea;

// 이번 프레임의 입력. 키 상태는 ReadInput 한 곳에서만 읽고, 나머지는 이 값만 본다
// (예전에는 같은 C 키를 상태 갱신과 무기 갱신이 따로 해석했다)
struct tPlayerInput
{
    bool bLeft        = false;  // ← 누르고 있다
    bool bRight       = false;  // →
    bool bUp          = false;  // ↑ 누르고 있다 (위 보기)
    bool bUpRelease   = false;
    bool bDown        = false;  // ↓ 누르고 있다 (앉기, 공중이면 아래 보기)
    bool bDownRelease = false;
    bool bJump        = false;  // X 누른 순간
    bool bFireTap     = false;  // C 누른 순간
    bool bFireHold    = false;  // C 누르고 있다 (누른 순간 다음 프레임부터)
    bool bBomb        = false;  // Z 누른 순간

    // 디버그 키 (Debug 빌드만)
    bool bDebugVictory = false; // F
    bool bDebugGod     = false; // V
};

// 플레이어
// tick 흐름: 입력 읽기 -> 이동 -> 몸 상태(상체·하체·점프·방향) -> 애니메이션 -> 총구·충돌체 -> 무기 -> 사망
class CPlayer :
    public CObj
{
public:
    // 상체·하체 상태 (예전에는 전역이었고 CWeaponEffect.h의 전역 STATE와 이름이 겹쳤다)
    enum class STATE
    {
        IDLE,
        RUN,
        ATTACK,
        BOMB,
        DEAD,
    };

    // 점프·앉기 상태
    enum class UPDOWN
    {
        JUMP,
        IDLE,
        BOW,
    };

private:
    // 총구 자리: 몸 기준점에서 총알 그림의 왼쪽 위까지. 무기마다 다르다 (data\player.ini [muzzle_pistol]·[muzzle_hmg])
    struct tMuzzle
    {
        Vec2  vUpLeft;           // 위를 볼 때 (왼쪽·오른쪽을 보고 있던 쪽)
        Vec2  vUpRight;
        Vec2  vDown;             // 공중에서 아래를 볼 때
        Vec2  vLeft;             // 옆 (폭탄·칼은 위·아래를 봐도 이 자리)
        Vec2  vRight;
        float fRaise    = 0.f;   // 그다음 몸 높이의 이만큼 위로 (서 있을 때)
        float fBowRaise = 0.f;   // 앉았을 때
    };

    // 플레이어 수치 (data\player.ini, 스테이지 끝은 stage.ini). 만들 때 읽는다 (스테이지에 들어올 때마다 새로 만든다)
    struct tData
    {
        float   fSpeed               = 0.f;   // [move]
        float   fBowSpeedDiv         = 0.f;
        float   fJumpSpeed           = 0.f;
        float   fMoveDetectSpeed     = 0.f;
        float   fCameraRatio         = 0.f;   // [camera]
        float   fScrollDiv           = 0.f;
        float   fBowScrollDiv        = 0.f;
        float   fPushbackSpeed       = 0.f;
        float   fStageEndX           = 0.f;
        Vec2    vStandSize;                   // [body]
        Vec2    vStandCenter;
        Vec2    vBowSize;
        Vec2    vBowCenter;
        float   fHitboxRatio         = 0.f;
        int     iLifeCount           = 0;     // [life]
        Vec2    vSpawnPos;
        float   fSummonTime          = 0.f;
        float   fDeadTime            = 0.f;
        float   fInvincibleTime      = 0.f;
        int     iStartBullet         = 0;     // [ammo]
        int     iStartBomb           = 0;
        int     iRespawnBullet       = 0;
        int     iRespawnBomb         = 0;
        int     iItemBullet          = 0;
        int     iItemBomb            = 0;
        float   fHoldInterval        = 0.f;   // [attack]
        float   fCQCHoldInterval     = 0.f;
        float   fPistolInterval      = 0.f;
        float   fHMGInterval         = 0.f;
        float   fKnifeInterval       = 0.f;
        float   fPistolPoseTime      = 0.f;
        float   fHMGPoseTime         = 0.f;
        float   fKnifePoseTime       = 0.f;
        float   fHMGBowKnifePoseTime = 0.f;
        float   fBombPoseTime        = 0.f;
        float   fCQCAreaOffset       = 0.f;
        tMuzzle tPistol;
        tMuzzle tHMG;
    };
    static tData LoadData();

    const tData m_tData;

    float m_fInvincibleTime = 0.f;
    float m_fPlayerDir      = 0.f;
    float m_fCQCDelay       = 0.f;
    Vec2  m_vResolution;
    bool  m_bHMG = false;
    bool  m_bCQC = false;

    tPlayerInput m_tInput;

    CCQCArea* m_pCQC = nullptr;
    DIR       m_tDir        = DIR::NONE;    // 위·아래를 보고 있으면 DOWN (총알을 세로로 쏜다)
    STATE     m_tUpperState = STATE::IDLE;  // 상체 상태 : 기본, 달리기, 사격, 폭탄
    STATE     m_tLowerState = STATE::IDLE;  // 하체 상태 : 기본, 달리기

    DIR m_tVRT_Dir     = DIR::NONE;
    DIR m_tHRZ_Dir     = DIR::RIGHT;
    DIR m_tShiftVRTDir = DIR::NONE;         // 이번 tick을 시작할 때의 값 (바뀌면 애니메이션을 다시 고른다)
    DIR m_tShiftHRZDir = DIR::RIGHT;

    STATE m_tShiftUpperState = STATE::IDLE;
    STATE m_tShiftLowerState = STATE::IDLE;

    bool m_bPlayerDir = true; // 몸체가 이동방향을 따라가게 함. (true = 오른쪽)

    UPDOWN m_tJumpBow      = UPDOWN::IDLE; // 점프·앉기 상태
    UPDOWN m_tShiftJumpBow = UPDOWN::IDLE;

    float m_fAttackTime  = 0.f;
    float m_fAttackDelay = 0.f; // 공격 애니메이션 지속시간
    bool  m_bAttackOn    = false;

    bool  m_bBombOn    = false;
    bool  m_bThrowBomb = false;   // 이번 프레임에 폭탄을 만든다 (몸 상태가 요청, 무기가 처리)
    float m_fBombDelay = 0.f;
    Vec2  m_vPrevPos; // 이전 위치 (화면)

    int   m_iBulletCount = 0;       // HMG 탄 (권총은 무한)
    int   m_iBombCount   = 0;
    int   m_iLifeCount   = 0;       // 남은 목숨 (화면에 보이는 수. 0이면 마지막 목숨이다)
    bool  m_bSummon       = true;
    bool  m_bStart       = true;
    bool  m_bDead        = false;
    bool  m_bGameOver    = false;   // 마지막 목숨에서 죽고 사망 연출이 끝났다 (스테이지가 게임 오버로 보낸다)
    bool  m_bVictory     = false;   // 보스를 이겼다: 승리 포즈로 서서 조작을 받지 않고 죽지 않는다 (스테이지가 타이틀로 보낸다)
    bool  m_bVictoryPose = false;   // 승리 포즈를 시작했다 (죽어 있었으면 다시 나온 뒤에 시작한다)
    bool  m_bGod         = false;
    float m_fDeadTime    = 0.f;
    float m_fSummonTime   = 0.f;

private:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual void BeginOverlap(CCollider* _pOther) override;

    void ReadInput();
    void UpdateMovement();          // 좌우 이동·카메라 밀기·점프
    void MoveHorizontal();
    void UpdateBodyState();         // 상체·하체·점프·방향 상태, 사격·폭탄 요청
    void UpdateAnimation();         // 상태가 바뀌었으면 애니메이션을 다시 고른다
    Vec2 GetMuzzlePos(bool _bAim) const;   // 총알이 나오는 자리(_bAim 참: 위·아래 조준을 따른다), 폭탄·칼이 나오는 손 자리(거짓)
    void UpdateCollider();          // 앉으면 충돌체가 낮아진다
    void UpdateWeapon(const Vec2& _vMuzzle, const Vec2& _vHand);
    void UpdateDeath();             // 사망 연출이 끝나면 다시 나오거나(목숨을 하나 쓴다), 목숨이 없으면 게임 오버
    void Respawn();
    void ResetBodyState();          // 몸 상태·방향·무기 자세를 처음으로 (서서 오른쪽을 본다)
    void FireBullet(const Vec2& _vMuzzle);   // 권총·HMG 총알 한 발
    void ChangeWeapon(bool _bHMG);           // 권총·HMG를 바꾸고 상체 그림도 바로 그 무기로

    void PlayUpperAnim();
    void PlayLowerAnim();
    void PlayAnim(const wstring& _strName, SETPRINT _tUpDownNormal, bool _bRepeat, bool _bDirection);
    void LoadAnim() const;

public:
    void SetHorizonDir(DIR _HRZDir)
    {
        m_tHRZ_Dir   = _HRZDir;
        m_fPlayerDir = static_cast<float>(_HRZDir) * 90.f;
    }

    void SetVerticalDir(DIR _VRTDir)
    {
        m_tVRT_Dir   = _VRTDir;
        m_fPlayerDir = static_cast<float>(_VRTDir) * 90.f;
    }

    void  SetDir(DIR _Dir) { m_fPlayerDir = static_cast<float>(_Dir) * 90.f; }
    int   GetBombCount() const { return m_iBombCount; }
    int   GetBulletCount() const { return m_iBulletCount; }
    int   GetLifeCount() const { return m_iLifeCount; }
    bool  IsGameOver() const { return m_bGameOver; }
    bool  IsHMG() const { return m_bHMG; }
    void  SetHMG() { ChangeWeapon(true); }

    // 소환 연출부터 다시 시작한다 (스테이지 시작, 떨어졌을 때, 죽은 뒤). 이전 몸 상태는 남기지 않는다
    void SetSummon()
    {
        m_bSummon     = true;
        m_bStart     = true;
        m_fSummonTime = 0.f;     // 두 번째 소환부터 연출이 같은 tick에 끝나지 않게
        ResetBodyState();
    }

    // 시간 초과 등으로 바로 죽인다 (무적 디버그 설정일 때, 보스를 이긴 뒤는 제외)
    void Kill()
    {
        if (!m_bDead && !m_bGod && !m_bVictory)
            m_bDead = true;
    }

    // 보스를 이겼다 (CBossWreck). 살아 있으면 다음 tick부터 승리 포즈
    void SetVictory() { m_bVictory = true; }
    bool IsVictory() const { return m_bVictoryPose; }

    void AddBullet() { m_iBulletCount += m_tData.iItemBullet; }
    void AddBomb() { m_iBombCount += m_tData.iItemBomb; }
    void ToggleGod() { m_bGod = !m_bGod; }

    // 자동 테스트용
    void SetGodMode(const bool _bGod) { m_bGod = _bGod; }
    void SetHMGMode(const bool _bHMG) { ChangeWeapon(_bHMG); }

public:
    // 소환되는 화면 위치 (스테이지 시작, 다시 살아날 때)
    Vec2 GetSpawnScreenPos() const { return m_tData.vSpawnPos; }

public:
    CLONE_DEACTIVATE(CPlayer);

public:
    CPlayer();
    CPlayer(const CPlayer&) = delete;
    virtual ~CPlayer() override;
};
