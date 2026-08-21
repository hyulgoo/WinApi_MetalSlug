#pragma once
#include "Game\05.Component\CComponent.h"

class CCollider :
    public CComponent
{
private:
    Vec2 m_vOffsetPos;
    Vec2 m_vFinalPos;
    Vec2 m_vScale;
    int  m_iOverlapCount;
    bool m_bRender;

public:
    void SetOffsetPos(const Vec2& _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    void SetScale(const Vec2& _vScale) { m_vScale = _vScale; }

    Vec2 GetOffsetPos() { return m_vOffsetPos; }
    Vec2 GetScale() { return m_vScale; }

    Vec2 GetFinalPos() { return m_vFinalPos; }
    void SetRender();

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

public:
    CLONE(CCollider)

public:
    void BeginOverlap(CCollider* _pOther);
    void OnOverlap(CCollider* _pOther);
    void EndOverlap(CCollider* _pOther);

public:
    CCollider(CObj* _pOwner);
    CCollider(const CCollider& _other);
    ~CCollider();
};
