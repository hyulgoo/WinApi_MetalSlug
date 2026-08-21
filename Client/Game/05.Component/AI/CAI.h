#pragma once
#include "Game\05.Component\CComponent.h"

class CState;
class CMonster;

class CAI :
    public CComponent
{
private:
    map<wstring, CState*>   m_mapState;
    CState*                 m_pCurState;

public:
    virtual void tick() override {}
    virtual void final_tick() override;
    virtual void render(HDC _dc) override {}

public:
    void    AddState(const wstring& _strKey, CState* _pState);
    CState* GetState() const { return m_pCurState; }
    CState* FindState(const wstring& _strKey)
    {
        const map<wstring, CState*>::iterator iter = m_mapState.find(_strKey);
        if (iter != m_mapState.end())
            return iter->second;
        return nullptr;
    }
    bool GetMonDir() const;
    void ChangeState(const wstring& _strStateName);

    CLONE(CAI);

public:
    CAI(CObj* _pOWner);
    ~CAI();
};

