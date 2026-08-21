#pragma once
#include "CObj.h"
class CBoss :
    public CObj
{

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void OnOverlap(CCollider* _pOther) override;

    void LoadAnimation();
    void PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
    void Play(bool _bDir);
    LAYER GetLayer() { return LAYER::MONSTER; }
public:
    CBoss();
    ~CBoss();
};

