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
    float m_fLifeTime;   // 이 시간이 지나면 사라진다

    CTexture* m_pTexture;
    CTexture* m_pBackBuffer;
    float     m_fmagni;
    bool      m_bVRT = false;

    // 총알 그림 (texture\Weapone.bmp 안의 자리와 크기). 옆으로, 위·아래로 쏠 때
    Vec2      m_vSideLeftTop;
    Vec2      m_vSideSize;
    Vec2      m_vVertLeftTop;
    Vec2      m_vVertSize;

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
