#pragma once
#include "CObj.h"
class CTexture;

class CItem :
    public CObj
{
private:
    CTexture* m_pAtlas;
    int       m_iBulletBomb;
    bool      m_bStart;

public:
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetBB(const int _iBB) { m_iBulletBomb = _iBB; }

public:
    CItem();
    virtual ~CItem() override;
};
