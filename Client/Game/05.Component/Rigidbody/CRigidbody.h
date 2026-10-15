#pragma once
#include "Game\05.Component\CComponent.h"
class CRigidbody :
    public CComponent
{
private:
    Vec2    m_vForce;         // 힘
    Vec2    m_vVelocity;      // 속도
    float   m_fMass;          // 질량

    float   m_fFriction;      // 마찰력
    float   m_fFrictionScale; // 마찰 계수

    float   m_fVelocityLimit; // 제한 속도

    float   m_fGravityAccel;  // 중력 가속도 값
    float   m_fGravityVLimit;
    bool    m_bGravityUse;    // 중력 사용여부
    // 중력 가속과 별개로 늘 이 속도로 아래로 민다(땅에 서 있어도 민다. 픽셀 충돌이 다시 땅 위로 올려
    // 비탈을 내려갈 때도 땅에 붙어 있다). 예전에는 픽셀 충돌이 위치를 직접 옮겨 맡았다
    float   m_fFallSpeed;
    bool    m_bGround;        // 땅 체크

public:
    virtual void tick() override;
    virtual void final_tick() override;
    virtual void render(HDC _dc) override;

public:
    void AddForce(const Vec2& _vForce) { m_vForce += _vForce; }
    void AddVelocity(const Vec2& _vVelocity) { m_vVelocity += _vVelocity; }
    void SetVelocityLimit(const float _fLimit) { m_fVelocityLimit = _fLimit; }
    void SetGravityVelocityLimit(const float _fLimit) { m_fGravityVLimit = _fLimit; }

    void SetVelocity(const Vec2& _vVelocity) { m_vVelocity = _vVelocity; }
    Vec2 GetVelocity() { return m_vVelocity; }

    bool IsGround() const { return m_bGround; }

    void SetFriction(const float _f) { m_fFriction = _f; }
    void SetFrictionScale(const float _f) { m_fFrictionScale = _f; }
    void SetGravity(const bool _bSet) { m_bGravityUse = _bSet; }
    void SetGravityAccel(const float _fAccel) { m_fGravityAccel = _fAccel; }
    void SetFallSpeed(const float _fSpeed) { m_fFallSpeed = _fSpeed; }
    void SetGround(bool _bGround);

public:
    CLONE(CRigidbody);

public:
    CRigidbody(CObj* _pOwner);
    CRigidbody(const CRigidbody& _other);
    virtual ~CRigidbody();

};

