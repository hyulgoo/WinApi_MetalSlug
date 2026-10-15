#pragma once
#include "Game\04.Object\CObj.h"

// 부서진 보스: 돔이 깨진 받침대(BOSS_WRECK)를 남기고, 그 위로 폭발이 이어진다.
// 폭발이 끝나고 잠시 뒤 플레이어가 승리 포즈를 한다 (스테이지가 그것을 보고 타이틀로 보낸다).
// 위치는 보스와 같다 (몸 그림의 가운데). 수치는 data\boss.ini [wreck]
class CBossWreck :
    public CObj
{
private:
    struct tData
    {
        float fExplosionTime     = 0.f;
        float fExplosionInterval = 0.f;
        Vec2  vExplosionArea;          // 폭발이 생기는 상자 (가운데에서)
        Vec2  vExplosionOffset;
        float fShakeRange        = 0.f;
        float fShakeSpeed        = 0.f;
        float fVictoryDelay      = 0.f;
    };
    tData m_tData;

    float m_fTime          = 0.f;
    float m_fNextExplosion = 0.f;   // 다음 폭발 시각
    int   m_iExplosion     = 0;
    bool  m_bStart         = false;
    bool  m_bVictory       = false; // 플레이어에게 이겼다고 알렸다

public:
    virtual void tick() override;

private:
    void SpawnExplosion();

public:
    CLONE_DEACTIVATE(CBossWreck);

public:
    CBossWreck();
    CBossWreck(const CBossWreck&) = delete;
    virtual ~CBossWreck() override;
};
