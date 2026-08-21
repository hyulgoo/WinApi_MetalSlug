#pragma once
#include "CMonster.h"

class CState;

class CSoldier :
    public CMonster
{
private:
    CState*     m_pCurState;
    CState*     m_pPrevState;
    CTexture*   m_pAtlas;
    bool        m_bDir;
    float       m_fAttackTime;
    float       m_fCQCTime;
    bool        m_bFire;
    bool        m_bStart;


public:
    void init();
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void OnOverlap(CCollider* _pOther) override;

    void          LoadAnimation();
    void          PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
    void          Play(bool _bDir);
    virtual LAYER GetLayer() override { return LAYER::MONSTER; }
    CLONE(CSoldier);
public:
    CSoldier();
    CSoldier(const CSoldier& _other);
    virtual ~CSoldier() override;
};

