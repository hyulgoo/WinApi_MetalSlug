#pragma once
#include "CEntity.h"

class CObj;

class CComponent :
    public CEntity
{
private:
    CObj* m_pOwner;

public:
    virtual void tick() = 0;
    virtual void final_tick() {};
    virtual void render(HDC _dc) = 0;

public:
    CObj* GetOwner() { return m_pOwner; }

private:
    void SetOwner(CObj*_other) { m_pOwner = _other; }

public:
    CComponent(CObj* _pOwner);
    ~CComponent();

    friend class CObj;
};

