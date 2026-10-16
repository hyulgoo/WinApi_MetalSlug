#pragma once
#include "Game\04.Object\CObj.h"

// 비석 (보스가 조준 빔 자리에 떨어뜨린다). 화면 위 밖에서 떨어져 땅에 박히고, 그 순간 그 자리에 있으면 깔린다(CCrushArea).
// 박힌 뒤에는 플레이어를 옆에서 막고 위에 올라설 수 있으며(OBSTACLE 레이어의 상자), 잠시 뒤부터 천천히 가라앉는다(MONOLITH.anim 33장).
// 총알·수류탄에 맞으면 그때마다 바로 더 가라앉고(기다리는 동안에도), 다 가라앉으면 돌무더기를 잠깐 남기고 사라진다.
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
        float fHoldTime          = 0.f;   // 박힌 뒤 저절로 가라앉기 시작할 때까지
        float fSinkFrameTime     = 0.f;   // 저절로 가라앉는 그림 한 장의 시간
        float fSinkBullet        = 0.f;   // 맞으면 바로 이만큼(그림 장 수) 더 가라앉는다
        float fSinkGrenade       = 0.f;
        float fCollapseFrameTime = 0.f;   // 보스가 부서진 뒤 가라앉는 그림 한 장의 시간
        float fTopInset          = 0.f;   // 밟는 윗면을 그림 위 끝에서 이만큼 아래로
    };
    tData m_tData;

    float m_fGroundY  = 0.f;    // 박히는 땅 (월드 y)
    float m_fTime     = 0.f;    // 박힌 뒤 흐른 시간
    int   m_iLandTick = 0;      // 박힌 뒤 지난 tick 수 (밟고 막기 시작하는 때를 정한다)
    float m_fSunk     = 0.f;    // 가라앉은 정도 (그림 장 수, 소수 아래는 다음 장까지 남은 몫)
    bool  m_bStart    = false;
    bool  m_bLanded   = false;
    bool  m_bCollapse = false;  // 보스가 부서졌다: 깔리지 않고 빨리 가라앉는다

public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual bool GetObstacleBox(Vec2& _vLeftTop, Vec2& _vRightBottom) const override;

    void Collapse();   // 보스가 부서졌다: 기다리지 않고 빨리 가라앉는다 (아직 떨어지는 중이면 깔지 않는다)

private:
    void     Land();
    void     Sink(float _fFrames);   // 이만큼(그림 장 수) 더 가라앉는다
    int      FrmCount() const;
    int      SunkFrames() const;     // 가라앉은 그림 수 = 지금 보이는 그림 번호 (다 가라앉으면 FrmCount)
    tAnimFrm CurFrm() const;         // 지금 보이는 그림 (박히기 전에는 0번)
    void     UpdateCollider();

public:
    CLONE_DEACTIVATE(CMonolith);

public:
    CMonolith();
    CMonolith(const CMonolith&) = delete;
    virtual ~CMonolith() override;
};
