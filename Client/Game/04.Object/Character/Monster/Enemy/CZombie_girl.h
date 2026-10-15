#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

class CState;

class CZombie_girl :
    public CMonster
{
 private:
     CState*     m_pCurState = nullptr;
     CState*     m_pPrevState = nullptr;
     CTexture*   m_pAtlas = nullptr;
     bool        m_bDir = false;
     float       m_fDeadTime = 0.f;
     float       m_fAttackTime = 0.f;
     bool        m_bFire = false;

 public:
     void init();
     virtual void tick() override;
     virtual void render(HDC _dc) override;
     virtual void BeginOverlap(CCollider* _pOther) override;
     virtual void OnOverlap(CCollider* _pOther) override;

     void          LoadAnimation();
     void          PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
     void          PlayEffect(const wstring& _strName, bool _bRepeat, bool _bDirRight);
     void          Play(bool _bDir);
     virtual LAYER GetLayer() override { return LAYER::MONSTER; }

     CLONE_DEACTIVATE(CZombie_girl);
 public:
     CZombie_girl();
     CZombie_girl(const CZombie_girl&) = delete;  // 복제하지 않는다
     virtual ~CZombie_girl() override;
 };

