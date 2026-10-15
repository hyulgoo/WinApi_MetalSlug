#pragma once
#include "Game\04.Object\CObj.h"

// 깔리는 판정: 비석이 땅에 박히는 순간 그 자리를 덮는다. MONSTER_PROJECTILE 레이어라 플레이어가 겹치면 죽는다.
// 위치는 판정 상자의 가운데. _fLifeTime이 지나면 사라진다 (한 프레임이면 충분하다. 병사 칼 CSLDCQC와 같은 방식)
class CCrushArea :
    public CObj
{
private:
    float m_fTime     = 0.f;
    float m_fLifeTime = 0.f;

public:
    virtual void tick() override;

public:
    CLONE_DEACTIVATE(CCrushArea);

public:
    CCrushArea(const Vec2& _vSize, float _fLifeTime);
    CCrushArea(const CCrushArea&) = delete;
    virtual ~CCrushArea() override;
};
