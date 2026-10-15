#pragma once
#include "Game\04.Object\CObj.h"

// 권총 총알·플레이어 수류탄이 남기는 이펙트. 애니메이션이 끝나면 사라진다
class CWeaponEffect :
    public CObj
{
public:
    enum class KIND
    {
        GROUND,   // 총알이 땅에 맞음
        HIT,      // 총알이 적에 맞음
        WALL,     // 총알이 벽에 맞음
        BOMB,     // 수류탄 폭발
    };

private:
    bool m_bDir;
    bool m_bStart;
    KIND m_eKind;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void SetKind(const KIND _eKind) { m_eKind = _eKind; }

public:
    CWeaponEffect();
    virtual ~CWeaponEffect() override;
};
