#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

// 보스: 모세의 십계 (메탈슬러그 3 미션 2). 1페이즈(초록 돔이 비석을 떨어뜨린다)까지 구현했다.
// 스테이지 끝 돌무덤 자리(레벨 파일의 BOSS)에서 솟아오르고(APPEAR), 기다리다(WAIT) 돔이 빛기둥을 쏘면(ATTACK)
// 하늘에서 조준 빔이 플레이어를 따라오다 멈춘 자리에 비석이 떨어진다(CMonolithBeam -> CMonolith).
// 부서지면 잔해(CBossWreck)를 남기고, 폭발이 끝나면 플레이어가 승리 포즈를 한다.
// 위치는 몸 그림(BOSS_IDLE)의 가운데, 발밑은 그 아래 끝이다. 수치는 data\boss.ini
class CBoss :
    public CMonster
{
private:
    struct tData
    {
        float fFirstWaitTime = 0.f;   // [boss]
        float fWaitTime      = 0.f;
        int   iSummonFrame   = 0;
        float fAppearTime    = 0.f;   // [appear]
        float fAppearDepth   = 0.f;
        float fShakeRange    = 0.f;
        float fShakeSpeed    = 0.f;
        float fBurstInterval = 0.f;
        float fBurstSpread   = 0.f;
        COLORREF tFlashColor    = 0;     // [hit_flash]
        float    fFlashTime     = 0.f;
        float    fFlashInterval = 0.f;
    };
    tData m_tData;

    Vec2  m_vHome;              // 다 올라왔을 때의 위치 (레벨 파일의 자리)
    bool  m_bPlaced    = false; // 첫 tick에 자리를 정했다
    float m_fNextBurst = 0.f;   // 다음 먼지 기둥을 일으킬 등장 시각
    int   m_iBurst     = 0;
    bool  m_bAttacked  = false; // 한 번 공격했다 (기다리는 시간이 first_wait_time에서 wait_time으로)
    float m_fFlash     = 0.f;   // 피격 점멸이 남은 시간 (맞을 때마다 [hit_flash] time부터 다시)

public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;

    virtual void              PlayStateAnim(MON_STATE _eState) override;
    virtual const CAnimation* GetAttackAnim() const override;
    virtual int               GetAttackFrame(MON_STATE _eState) const override;
    virtual void              OnAttackFrame(MON_STATE _eState) override;
    virtual void              NextAttackCycle() override;
    virtual float             GetStateDuration(MON_STATE _eState) const override;
    virtual void              Emerge(float _fRatio) override;

protected:
    virtual void SpawnDeathEffect() override;

private:
    float GetFeetY() const { return m_vHome.y + GetScale().y / 2.f; }   // 다 올라왔을 때 발밑 (월드 y)
    void  PlayIfNot(CAnimator* _pAnimator, const wchar_t* _szName, bool _bRepeat);
    void  UpdateFlash();   // 피격 점멸: 켤 차례면 몸·돔 그림의 맞는 범위 높이를 빨갛게, 아니면 원래 색으로

public:
    CLONE_DEACTIVATE(CBoss);

public:
    CBoss();
    CBoss(const CBoss&) = delete;
    virtual ~CBoss() override;
};
