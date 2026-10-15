#pragma once
#include "Game\04.Object\CObj.h"

enum class STATE
{
    IDLE,
    RUN,
    ATTACK,
    BOMB,
    DEAD,
};

enum class UPDOWN
{
    JUMP,
    IDLE,
    BOW,
};

class CTexture;
class CAnimator;
class CCQCArea;

class CPlayer :
    public CObj
{
private:
    UINT  m_arrDir[static_cast<int>(DIR::DOWN)];
    float m_fSpeed          = 330.f;
    float m_fInvincibleTime = 0.f;
    float m_fPlayerDir      = 0.f;
    float m_fCameraRatio    = 0.f;
    float m_fCQCDelay       = 0.f;
    Vec2  m_vResolution;
    Vec2  m_vPrevLook;
    Vec2  m_vZero;
    bool  m_bHMG = false;
    bool  m_bCQC = false;

    CCQCArea* m_pCQC   = nullptr;
    CTexture* m_pAtlas = nullptr;
    DIR       m_tDir        = DIR::NONE;    // 현재 방향
    STATE     m_tUpperState = STATE::IDLE;  // 상체 상태 : 기본, 달리기, 사격
    STATE     m_tLowerState = STATE::IDLE;  // 하체 상태 : 기본, 달리기

    DIR m_tVRT_Dir     = DIR::NONE;
    DIR m_tHRZ_Dir     = DIR::RIGHT;
    DIR m_tShiftVRTDir = DIR::NONE;
    DIR m_tShiftHRZDir = DIR::RIGHT;
    DIR m_tPrevVRTDir  = DIR::NONE;
    DIR m_tPrevHRZDir  = DIR::RIGHT;

    STATE m_tPrevUpperState  = STATE::IDLE;
    STATE m_tPrevLowerState  = STATE::IDLE;
    STATE m_tShiftUpperState = STATE::IDLE; // 전환할 상체 상태
    STATE m_tShiftLowerState = STATE::IDLE; // 전환할 하체 상태

    bool m_bPlayerDir = true; // 몸체가 이동방향을 따라가게 함. (true = 오른쪽)

    UPDOWN m_tJumpBow      = UPDOWN::IDLE; // 점프 상태
    UPDOWN m_tShiftJumpBow = UPDOWN::IDLE;
    UPDOWN m_tPrevJumpBow  = UPDOWN::IDLE; // 이전 점프상태
    float  m_fUpJumpBow    = 0.f;          // 위로 인식되는 시간

    float m_fAttackTime  = 0.f;
    float m_fAttackDelay = 0.f; // 공격 애니메이션 지속시간
    bool  m_bAttackOn    = false;

    bool  m_bBombOn    = false;
    bool  m_bThrowBomb = false;   // 이번 프레임에 폭탄을 만든다 (RenewalDir이 요청, RenewalAttack이 처리)
    float m_fBombDelay = 0.f;
    Vec2  m_vPrevPos; // 이전 위치

    int   m_iBulletCount = 20;
    int   m_iBombCount   = 10;
    int   m_iLiftCount   = 3;
    bool  m_bSumon       = true;
    bool  m_bStart       = true;
    bool  m_bDead        = false;
    bool  m_bEnd         = false;
    bool  m_bGod         = false;
    float m_fDeadTime    = 0.f;
    float m_fSumonTime   = 0.f;

private:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void OnOverlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

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
    float GetCameraRatio() const { return m_fCameraRatio; }
    int   GetBombCount() const { return m_iBombCount; }
    int   GetBulletCount() const { return m_iBulletCount; }
    int   GetLifeCount() const { return m_iLiftCount; }
    bool  IsHMG() const { return m_bHMG; }
    void  SetHMG() { m_bHMG = true; }

    void RenewalAttack(const Vec2& vPos);
    void RenewalPos();
    void FireBullet(const Vec2& _vMuzzle);   // 권총·HMG 총알 한 발
    void RenewalDir();
    void PlayUpperAnim();
    void PlayLowerAnim();
    void PlayAnim(const wstring& _strName, SETPRINT _tUpDownNormal, bool _bRepeat, bool _bDirection);
    void LoadAnim() const;
    void SetCQC(const bool _bCQC) { m_bCQC = _bCQC; }

    void SetSumon()
    {
        m_bSumon     = true;
        m_bStart     = true;
        m_fSumonTime = 0.f;     // 두 번째 소환부터 연출이 같은 tick에 끝나지 않게
    }

    // 시간 초과 등으로 바로 죽인다 (무적 디버그 설정일 때는 제외)
    void Kill()
    {
        if (!m_bDead && !m_bGod)
            m_bDead = true;
    }

    void AddBullet() { m_iBulletCount += 100; }
    void AddBomb() { m_iBombCount += 10; }
    void SetGod() { m_bGod = !m_bGod; }

    // 자동 테스트용
    void SetGodMode(const bool _bGod) { m_bGod = _bGod; }
    void SetHMGMode(const bool _bHMG) { m_bHMG = _bHMG; }

public:
    float         GetSpeed() const { return m_fSpeed; }

public:
    CLONE_DEACTIVATE(CPlayer);

public:
    void SetSpeed(const float _f) { m_fSpeed = _f; }

public:
    CPlayer();
    virtual ~CPlayer() override;
};
