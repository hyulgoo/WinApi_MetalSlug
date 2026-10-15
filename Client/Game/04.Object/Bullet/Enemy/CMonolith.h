#pragma once
#include "Game\04.Object\CObj.h"

// 비석 (보스가 조준 빔 자리에 떨어뜨린다). 화면 위 밖에서 떨어져 땅에 박히고, 그 순간 그 자리에 있으면 깔린다(CCrushArea).
// 박힌 뒤에는 위에 올라설 수 있고(OBSTACLE 레이어의 발판), 잠시 뒤부터 천천히 가라앉는다(MONOLITH.anim 33장).
// 총알·수류탄에 맞으면 더 빨리 가라앉고, 다 가라앉으면 돌무더기를 잠깐 남기고 사라진다.
// 위치는 비석 아래 끝(박힌 땅)의 x. 수치는 data\boss.ini [monolith]
class CMonolith :
    public CObj
{
private:
    struct tData
    {
        float fFallSpeed         = 0.f;
        float fCrushTime         = 0.f;
        float fShakeRange        = 0.f;
        float fShakeSpeed        = 0.f;
        float fShakeTime         = 0.f;
        float fHoldTime          = 0.f;   // 박힌 뒤 가라앉기 시작할 때까지
        float fSinkFrameTime     = 0.f;   // 가라앉는 그림 한 장의 시간
        float fHitBullet         = 0.f;   // 맞으면 가라앉기를 이만큼(초) 앞당긴다
        float fHitGrenade        = 0.f;
        float fCollapseFrameTime = 0.f;   // 보스가 부서진 뒤 가라앉는 그림 한 장의 시간
        float fTopInset          = 0.f;   // 밟는 윗면을 그림 위 끝에서 이만큼 아래로
    };
    tData m_tData;

    float m_fGroundY  = 0.f;    // 박히는 땅 (월드 y)
    float m_fTime     = 0.f;    // 박힌 뒤 흐른 시간 (맞으면 앞당긴다)
    bool  m_bStart    = false;
    bool  m_bLanded   = false;
    bool  m_bCollapse = false;  // 보스가 부서졌다: 깔리지 않고 빨리 가라앉는다

public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual bool GetPlatformTop(float _fX, float& _fTop) const override;

    void Collapse();   // 보스가 부서졌다: 기다리지 않고 빨리 가라앉는다 (아직 떨어지는 중이면 깔지 않는다)

private:
    void     Land();
    float    SunkFrames() const;   // 가라앉은 그림 수 (박힌 뒤 hold_time이 지나면 늘어난다)
    tAnimFrm CurFrm() const;       // 지금 보이는 그림 (박히기 전에는 0번)
    void     UpdateCollider();

public:
    CLONE_DEACTIVATE(CMonolith);

public:
    CMonolith();
    CMonolith(const CMonolith&) = delete;
    virtual ~CMonolith() override;
};
