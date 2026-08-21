#pragma once
#include "CObj.h"

class CTexture;

class CHandgun :
    public CObj
{
private:
    float m_fSpeed;
    float m_fDegree;

    float m_fTime;

    CTexture* m_pTexture;
    CTexture* m_pBackBuffer;
    float     m_fmagni;
    bool      m_bVRT;
    bool      m_bDir;

public:
    void SetSpeed(const float _f) { m_fSpeed = _f; }
    void SetDir(const float _fDegree) { m_fDegree = _fDegree; }

public:
    virtual LAYER GetLayer() override { return LAYER::PLAYER_PROJECTILE; }
    void          SetVRT() { m_bVRT = true; }
    void          SetDir(const bool _vLeft) { m_bDir = _vLeft; }

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CHandgun();
    virtual ~CHandgun() override;
};
