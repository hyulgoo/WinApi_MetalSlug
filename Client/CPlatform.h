#pragma once
#include "CObj.h"
class CPlatform :
    public CObj
{
private:

public:
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void OnOverlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    virtual LAYER GetLayer() { return LAYER::TILE; }

public:
    CLONE(CPlatform);

public:
    CPlatform();
    ~CPlatform();
};

