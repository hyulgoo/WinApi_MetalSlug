#pragma once
#include "Game\04.Object\CObj.h"

class CPlayer;
class CAnimation;
class CAnimator;

// 몬스터 공통: FSM(CAI)이 부르는 값과 동작, 피해, 사망, 방향.
// 종류마다 다른 것(상태별 애니메이션, 공격 시점과 공격, 사망 이펙트)은 파생 클래스가 정한다
class CMonster :
    public CObj
{
private:
    int   m_iHP;
    float m_fSpeed;
    float m_fAttackRange;
    float m_fDetectRange;
    bool  m_bDir;       // 플레이어가 오른쪽에 있다 (왼쪽을 보고 그린 그림은 뒤집는다)
    bool  m_bFire;      // 마지막으로 불(수류탄)에 맞았다 (불탄 사망 이펙트)
    int   m_iRand;      // 생성할 때 정한 난수 (아이템 종류)

    // 애니메이터마다 지금 그림이 오른쪽을 보고 그려졌는가 (방향을 다시 맞출 때 쓴다)
    bool  m_bFacesRight[2] = {};

public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    int   GetHP() const { return m_iHP; }
    float GetSpeed() const { return m_fSpeed; }
    float GetAttackRange() const { return m_fAttackRange; }
    float GetDetectRange() const { return m_fDetectRange; }
    bool  GetDir() const { return m_bDir; }
    bool  IsFire() const { return m_bFire; }
    int   GetRand() const { return m_iRand; }

    // FSM이 쓰는 동작
    CPlayer* FindPlayer() const;            // 레벨에 플레이어가 없으면 nullptr
    float    DistToPlayer() const;          // 플레이어가 없으면 아주 먼 거리
    void     FacePlayer();                  // 플레이어 쪽을 보고, 지금 애니메이션의 방향도 맞춘다
    void     MoveToPlayer(float _fSpeed);   // 플레이어 쪽(대각선 포함)으로 움직인다. 같은 자리면 움직이지 않는다
    void     NextAttackCycle();             // 공격이 끝났다: 범위 안이면 다시 공격을 고르고, 아니면 쫓는다
    void     Die();                         // 사망 이펙트를 남기고 사라진다

    // 종류마다 정한다
    virtual void              PlayStateAnim(MON_STATE _eState) = 0;
    virtual const CAnimation* GetAttackAnim() const = 0;              // 공격 시점과 길이를 정하는 애니메이션
    virtual int               GetAttackFrame(MON_STATE _eState) const = 0;
    virtual void              OnAttackFrame(MON_STATE _eState) = 0;   // 공격 프레임에 들어섰다
    virtual MON_STATE         ChooseAttack(float /*_fDist*/) const { return MON_STATE::ATTACK; }

protected:
    virtual void SpawnDeathEffect() = 0;
    void         SetStats(int _iHP, float _fSpeed, float _fAttackRange, float _fDetectRange);
    // 애니메이터 하나에 재생한다. _bFacesRight: 그림이 오른쪽을 보고 그려졌는가
    void         PlayAnim(CAnimator* _pAnimator, const wchar_t* _szName, bool _bRepeat, bool _bFacesRight);

private:
    void ApplyFacing(CAnimator* _pAnimator, bool _bFacesRight) const;

public:
    CLONE_DEACTIVATE(CMonster);

public:
    CMonster();
    CMonster(const CMonster&) = delete;
    virtual ~CMonster() override;
};
