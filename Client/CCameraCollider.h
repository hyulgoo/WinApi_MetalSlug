#pragma once
#include "CComponent.h"

class CTexture;
class CBackGround;

class CCameraCollider :
    public CComponent
{
private:
    Vec2            m_vOffsetPos;
    Vec2            m_vFinalPos;
    Vec2            m_vScale;
    Vec2            m_vResolution;


public:
    void SetOffsetPos(Vec2 _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    void SetScale(Vec2 _vScale) { m_vScale = _vScale; }

    Vec2 GetOffsetPos() { return m_vOffsetPos; }
    Vec2 GetScale() { return m_vScale; }

    Vec2 GetFinalPos() { return m_vFinalPos; }

public:
    virtual void tick() override;
    virtual void final_tick() override;
    virtual void render(HDC _dc) override;
    
public:
    CLONE(CCameraCollider);

public:
    void BeginOverlap(CCameraCollider* _pOther);
    void OnOverlap(CCameraCollider* _pOther);
    void EndOverlap(CCameraCollider* _pOther);


public:
    CCameraCollider(CObj* _pOwner);
    CCameraCollider(const CCameraCollider& _pOwner);
    ~CCameraCollider();
};

