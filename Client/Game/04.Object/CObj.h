#pragma once
#include "Game\Entity\CEntity.h"

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
    LAYER            m_eLevelLayer;   // 레벨에 등록된 레이어 (CLevel::AddObject가 정한다)

public:
    void SetPos(const Vec2& _v) { m_vPos = _v; }
    Vec2 GetPos() const { return m_vPos; }
    // 화면에 그릴 위치. 레벨이 카메라를 따르게 한 레이어(스테이지의 월드 오브젝트)는 카메라만큼 옮기고,
    // UI·타이틀·에디터처럼 화면에 고정된 오브젝트는 위치 그대로다
    Vec2 GetScreenPos() const;

    void SetScale(const Vec2& _v) { m_vScale = _v; }
    Vec2 GetScale() const { return m_vScale; }

    void CreateCollider();
    void CreateAnimator();
    void CreateAnimator2();
    void CreateRigidbody();
    void CreatePixelCollider();
    void CreateAI();
 

    CCollider*      GetCollider() const { return m_pCollider; }
    CAnimator*      GetAnimator() const { return m_pAnimator; }
    CAnimator*      GetAnimator2() const { return m_pAnimator2; }
    CRigidbody*     GetRigidbody() const { return m_pRigidbody; }
    CPixelCollider* GetPixelCollider() const { return m_pPixelCollider; }
    CAI*            GetAI() const { return m_pAI; }
    // 레벨에 등록된 레이어. 충돌 검사와 충돌 상대 구분이 모두 이 값을 쓴다
    // (예전에는 클래스마다 가상 함수로 따로 적어, 등록 레이어와 어긋난 CCQCArea는 MONSTER x CQCAREA 검사가 빈 레이어를 봤다)
    LAYER           GetLayer() const { return m_eLevelLayer; }

public:
    virtual void tick();
    virtual void render(HDC _dc);
    virtual void final_tick();

    virtual void BeginOverlap(CCollider* /*_pOther*/) {}
    virtual void OnOverlap(CCollider* /*_pOther*/) {}
    virtual void EndOverlap(CCollider* /*_pOther*/) {}
    
public:
    bool IsDead() const { return m_bDead; }
    void SetDead();

    CLONE_DEACTIVATE(CObj);

public:
    CObj();
    CObj(const CObj& _other);
    virtual ~CObj();

    friend class CEventMgr;
    friend class CLevel;
};

