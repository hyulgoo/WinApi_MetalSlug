#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

class CState;

class CSoldier :
    public CMonster
{
private:
    CState*     m_pCurState = nullptr;
    CState*     m_pPrevState = nullptr;
    CTexture*   m_pAtlas = nullptr;
    bool        m_bDir = false;
    float       m_fAttackTime = 0.f;
    float       m_fCQCTime;
    bool        m_bFire = false;
    bool        m_bStart = false;


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
    CLONE_DEACTIVATE(CSoldier);
public:
    CSoldier();
    CSoldier(const CSoldier&) = delete;  // 복제하지 않는다
    virtual ~CSoldier() override;
};

