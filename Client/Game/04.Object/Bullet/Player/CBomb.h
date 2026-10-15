#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

class CBomb :
    public CObj
{
public:
    bool  m_bDir;
    bool  m_bStart;
    float m_fBounceTime;   // 처음 튄 뒤 흐른 시간
    bool  m_bBounce;       // 땅에 한 번 닿아 튀었다
    bool  m_bAirborne;     // 튄 뒤 땅에서 떨어졌다 (다음에 닿으면 터진다)
    Vec2  m_vPower;

    static constexpr float BOUNCE_TIMEOUT = 1.f;


public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetDir(const bool _vLeft) { m_bDir = _vLeft; }
    void         AddVelocity(bool _bDir);
    void         Explode();     // 폭발 이펙트를 남기고 사라진다

public:
    CBomb();
    virtual ~CBomb() override;
};
