#pragma once
#include "CEntity.h"
#include "CCamera.h"

class CCollider;
class CAnimator;
class CRigidbody;
class CPixelCollider;
class CAI;


class CObj :
    public CEntity
{
private:

    Vec2             m_vPos;
    Vec2             m_vScale;

    CCollider*       m_pCollider;
    CAnimator*       m_pAnimator;
    CAnimator*       m_pAnimator2;
    CRigidbody*      m_pRigidbody;
    CPixelCollider*  m_pPixelCollider;
    CAI*             m_pAI;

    bool             m_bDead;

public:
    void SetPos(Vec2 _v) { m_vPos = _v; }
    Vec2 GetPos() { return m_vPos; }

    void SetScale(Vec2 _v) { m_vScale = _v; }
    Vec2 GetScale() { return m_vScale; }

    void CreateCollider();
    void CreateAnimator();
    void CreateAnimator2();
    void CreateRigidbody();
    void CreatePixelCollider();
    void CreateAI();
 

    CCollider* GetCollider() { return m_pCollider; }
    CAnimator* GetAnimator() { return m_pAnimator; }
    CAnimator* GetAnimator2() { return m_pAnimator2; }
    CRigidbody* GetRigidbody() { return m_pRigidbody; }
    CPixelCollider* GetPixelCollider() { return m_pPixelCollider; }
    CAI* GetAI() { return m_pAI; }
    virtual LAYER GetLayer() { return LAYER::DEFAULT; }


public:
    virtual void tick();
    virtual void render(HDC _dc);
    virtual void final_tick();

    virtual void BeginOverlap(CCollider* _pOther) {}
    virtual void OnOverlap(CCollider* _pOther) {}
    virtual void EndOverlap(CCollider* _pOther) {}
    
public:
    bool IsDead() { return m_bDead; }
    void SetDead();

    CLONE_DEACTIVATE(CObj);

public:
    CObj();
    CObj(const CObj& _other);
    virtual ~CObj();

    friend class CEventMgr;
};

