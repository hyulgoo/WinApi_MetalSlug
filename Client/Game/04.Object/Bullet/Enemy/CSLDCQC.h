#pragma once
#include "Game\04.Object\CObj.h"

class CSLDCQC :
    public CObj
{
private:
    bool  m_bStart;
    float m_fOffset;     // 보는 쪽으로 이만큼 앞 (data\weapon.ini [soldier_knife])
    bool  m_bDir;
    float m_fTime;
    float m_fLifeTime;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    void         SetDir(const bool _bRight) { m_bDir = _bRight; }


    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CSLDCQC();
    virtual ~CSLDCQC() override;
};
