#pragma once
#include "CCollider.h"

class CTexture;

class CPixelCollider :
    public CComponent
{
private:
    CTexture*   m_pTex;
    CTexture*   m_pAtlas;
    Vec2        m_vResolution;
    Vec2        m_vOffsetPos;

    Vec2        m_OwnerPos;
    Vec2        m_OwnerScale;

    Vec2        m_vFinalPos;
    float       m_fmagni;

    Vec2        m_vGroundPoint1;
    Vec2        m_vGroundPoint2;
    Vec2        m_vWallPoint1;
    Vec2        m_vWallPoint2;
    bool        m_bPlayPixel;
    bool        m_bJump;
    bool        m_bGround;
    bool        m_bWall;
    bool        m_bGravity;

public:
    void SetOffsetPos(const Vec2& _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    Vec2 GetOffsetPos() { return m_vOffsetPos; }
    bool IsJump() const { return m_bJump; }
    void SetPlayPixel()
    {
        if (m_bPlayPixel == false)
            m_bPlayPixel = true;
        else
            m_bPlayPixel = false;
    }

public:
    virtual void tick() override;
    virtual void final_tick() override;
    virtual void render(HDC _dc) override;
    bool         IsGround() const { return m_bGround; }
    bool         IsWall() const { return m_bWall; }
    void         SetGravity() { m_bGravity = false; }

    CLONE_DEACTIVATE(CPixelCollider);

public:
    CPixelCollider(CObj* _pOwner);
    ~CPixelCollider();
};

