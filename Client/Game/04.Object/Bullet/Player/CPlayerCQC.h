#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CPlayerCQC :
    public CObj
{
private:
    float m_fTime;


public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CPlayerCQC();
    virtual ~CPlayerCQC() override;
};
