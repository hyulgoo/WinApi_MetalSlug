#pragma once
#include "CObj.h"

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
    DIR       m_tDir;        // 현재 방향
    STATE     m_tUpperState; // 상체 상태 : 기본, 달리기, 사격
    STATE     m_tLowerState; // 하체 상태 : 기본, 달리기

    DIR m_tVRT_Dir;
    DIR m_tHRZ_Dir;
    DIR m_tShiftVRTDir;
    DIR m_tShiftHRZDir;
    DIR m_tPrevVRTDir;
    DIR m_tPrevHRZDir;

    STATE m_tPrevUpperState;
    STATE m_tPrevLowerState;
    STATE m_tShiftUpperState; // 전환할 상체 상태
    STATE m_tShiftLowerState; // 전환할 하체 상태

    bool m_bPlayerDir; // 몸체가 이동방향을 따라가게 함.

    UPDOWN m_tJumpBow; // 점프 상태
    UPDOWN m_tShiftJumpBow;
    UPDOWN m_tPrevJumpBow; // 이전 점프상태
    float  m_fUpJumpBow;   // 위로 인식되는 시간

    float m_fAttackTime;
    float m_fAttackDelay; // 공격 애니메이션 지속시간
    bool  m_bAttackOn;

    bool  m_bBombOn;
    float m_fBombDelay;
    Vec2  m_vPrevPos; // 이전 위치

    int   m_iBulletCount;
    int   m_iBombCount;
    int   m_iLiftCount;
    bool  m_bSumon;
    bool  m_bStart;
    bool  m_bDead;
    bool  m_bEnd;
    bool  m_bGod;
    float m_fDeadTime;
    float m_fSumonTime;

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
    void RenewalDir();
    void PlayUpperAnim();
    void PlayLowerAnim();
    void PlayAnim(const wstring& _strName, SETPRINT _tUpDownNormal, bool _bRepeat, bool _bDirection);
    void LoadAnim() const;
    void SetCQC(const bool _bCQC) { m_bCQC = _bCQC; }

    void SetSumon()
    {
        m_bSumon = true;
        m_bStart = true;
    }

    void AddBullet() { m_iBulletCount += 100; }
    void AddBomb() { m_iBombCount += 10; }
    void SetGod() { m_bGod = !m_bGod; }

public:
    virtual LAYER GetLayer() override { return LAYER::PLAYER; }
    float         GetSpeed() const { return m_fSpeed; }

public:
    CLONE_DEACTIVATE(CPlayer);

public:
    void SetSpeed(const float _f) { m_fSpeed = _f; }

public:
    CPlayer();
    virtual ~CPlayer() override;
};
