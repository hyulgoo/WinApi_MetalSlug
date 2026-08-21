#pragma once
#include "CObj.h"
class CGround :
    public CObj
{
private:

public:
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void OnOverlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);

public:
    CLONE(CGround);

public:
    CGround();
    ~CGround();
};

