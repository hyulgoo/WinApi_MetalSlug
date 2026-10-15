#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

class CBomb :
    public CObj
{
private:
    bool  m_bDir;
    bool  m_bStart;
    float m_fBounceTime;   // 처음 튄 뒤 흐른 시간
    bool  m_bBounce;       // 땅에 한 번 닿아 튀었다
    bool  m_bAirborne;     // 튄 뒤 땅에서 떨어졌다 (다음에 닿으면 터진다)
    Vec2  m_vPower;        // 던질 때 속도 (오른쪽으로 던질 때)

    // 수치 (data\weapon.ini [bomb]·[projectile]). 만들 때 읽는다
    float m_fBounceRatio;    // 튈 때 가로 속도 = 던질 때 가로 속도 x 이 값
    float m_fBounceTimeout;  // 튄 뒤 이 시간 안에 땅에 다시 닿지 못하면 그 자리에서 터진다
    float m_fRemoveLeft;     // 화면 밖으로 이만큼 나가면 지운다
    float m_fRemoveRight;
    float m_fRemoveBottom;


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
