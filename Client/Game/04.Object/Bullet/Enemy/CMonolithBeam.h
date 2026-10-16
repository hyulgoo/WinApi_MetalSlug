#pragma once
#include "Game\04.Object\CObj.h"

// 조준 빔 (보스가 부른다): 하늘에서 내려온 가는 빔이 플레이어를 따라오다 멈추고, 사라지면서 그 자리에 비석(CMonolith)을 떨어뜨린다.
// 빔이 닿은 땅에는 표식이 깜빡인다. 위치는 빔이 땅에 닿는 자리. 수치는 data\boss.ini [beam]
class CMonolithBeam :
    public CObj
{
private:
    struct tData
    {
        float fTrackTime  = 0.f;   // 플레이어를 따라오는 시간
        float fTrackSpeed = 0.f;   // 따라오는 최고 속도
        float fLockTime   = 0.f;   // 멈춘 뒤 사라지며 비석을 떨어뜨릴 때까지
    };
    tData m_tData;

    float m_fTime    = 0.f;
    bool  m_bStart   = false;

public:
    virtual void tick() override;

public:
    CLONE_DEACTIVATE(CMonolithBeam);

public:
    CMonolithBeam();
    CMonolithBeam(const CMonolithBeam&) = delete;
    virtual ~CMonolithBeam() override;
};
