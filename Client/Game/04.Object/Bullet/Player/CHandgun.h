#pragma once
#include "Game\04.Object\CObj.h"

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
    bool      m_bVRT = false;

public:
    void SetSpeed(const float _f) { m_fSpeed = _f; }
    void SetDir(const float _fDegree) { m_fDegree = _fDegree; }

public:
    void          SetVRT() { m_bVRT = true; }
    Vec2          GetSpriteSize() const;    // 화면에 그리는 총알 그림의 크기

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CHandgun();
    virtual ~CHandgun() override;
};
