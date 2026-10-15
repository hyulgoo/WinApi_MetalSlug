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
    float m_fSpeed          = 330.f;
    float m_fInvincibleTime = 0.f;
    float m_fPlayerDir      = 0.f;
    float m_fCameraRatio    = 0.45f;   // 화면의 이 비율보다 오른쪽으로 가면 카메라를 민다
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

    int   m_iBulletCount = 20;
    int   m_iBombCount   = 10;
    int   m_iLifeCount   = 3;
    bool  m_bSummon       = true;
    bool  m_bStart       = true;
    bool  m_bDead        = false;
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
    Vec2 GetMuzzlePos() const;      // 총알·칼·폭탄이 나오는 자리
    void UpdateCollider();          // 앉으면 충돌체가 낮아진다
    void UpdateWeapon(const Vec2& _vMuzzle);
    void UpdateDeath();
    void FireBullet(const Vec2& _vMuzzle);   // 권총·HMG 총알 한 발

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
    bool  IsHMG() const { return m_bHMG; }
    void  SetHMG() { m_bHMG = true; }

    void SetSummon()
    {
        m_bSummon     = true;
        m_bStart     = true;
        m_fSummonTime = 0.f;     // 두 번째 소환부터 연출이 같은 tick에 끝나지 않게
    }

    // 시간 초과 등으로 바로 죽인다 (무적 디버그 설정일 때는 제외)
    void Kill()
    {
        if (!m_bDead && !m_bGod)
            m_bDead = true;
    }

    void AddBullet() { m_iBulletCount += 100; }
    void AddBomb() { m_iBombCount += 10; }
    void ToggleGod() { m_bGod = !m_bGod; }

    // 자동 테스트용
    void SetGodMode(const bool _bGod) { m_bGod = _bGod; }
    void SetHMGMode(const bool _bHMG) { m_bHMG = _bHMG; }

public:
    float GetSpeed() const { return m_fSpeed; }

    // 소환되는 화면 위치 (스테이지 시작, 다시 살아날 때)
    static constexpr Vec2 SPAWN_SCREEN_POS = Vec2(100.f, 500.f);

public:
    CLONE_DEACTIVATE(CPlayer);

public:
    CPlayer();
    CPlayer(const CPlayer&) = delete;
    virtual ~CPlayer() override;
};
