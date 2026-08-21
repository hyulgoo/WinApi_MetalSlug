#pragma once
#include "Game\04.Object\CObj.h"

class CBoss :
    public CObj
{
public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void OnOverlap(CCollider* _pOther) override;

    void          LoadAnimation() const;
    void          PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight) const;
    void          Play(bool _bDir) const;
    virtual LAYER GetLayer() override { return LAYER::MONSTER; }

public:
    CBoss();
    virtual ~CBoss() override;
};
