#pragma once
#include "Game\04.Object\CObj.h"
class CTexture;

class CItem :
    public CObj
{
private:
    int       m_iBulletBomb;   // 몬스터가 생길 때 뽑아 둔 수
    int       m_iHMGFrom;      // m_iBulletBomb이 이 값 이상이면 HMG 상자, 아니면 폭탄 상자 (data\monster.ini [item])
    bool      m_bStart;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    void         SetBB(const int _iBB) { m_iBulletBomb = _iBB; }

public:
    CItem();
    virtual ~CItem() override;
};
