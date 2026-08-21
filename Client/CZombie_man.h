#pragma once
#include "CMonster.h"

class CState;

class CZombie_man :
    public CMonster
{
    private:
        CState*     m_pCurState;
        CState*     m_pPrevState;
        CTexture*   m_pAtlas;
        bool        m_bDir;
        float       m_fDeadTime;
        float       m_fAttackTime;
        bool        m_bFire;

    public:
        void init();
        virtual void tick() override;
        virtual void render(HDC _dc) override;
        virtual void BeginOverlap(CCollider* _pOther) override;
        virtual void OnOverlap(CCollider* _pOther) override;

        void LoadAnimation();
        void PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
        void PlayEffect(const wstring& _strName, bool _bRepeat, bool _bDirRight);
        void Play(bool _bDir);
        LAYER GetLayer() { return LAYER::MONSTER; }
        CLONE(CZombie_man);
    public:
        CZombie_man();
        CZombie_man(const CZombie_man& _other);
        ~CZombie_man();
 };
