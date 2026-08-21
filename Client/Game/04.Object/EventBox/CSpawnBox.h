#pragma once
#include "Game\04.Object\CObj.h"

class CSpawnBox :
    public CObj
{
private:
    map<wstring, CObj*> m_mapObj;
    MONSTER             m_strMon;
    Vec2                m_vMonPos;

public:
    virtual void  tick() override;
    virtual void  render(HDC _dc) override;
    virtual LAYER GetLayer() override { return LAYER::EDITUI; }
    CLONE(CSpawnBox)

    void AddMonster(const wstring& _strKey, CObj* _pState) 
    {
        m_mapObj.insert(make_pair(_strKey, _pState));
    }

    CObj* FindMonster(const wstring& _strKey)
    {
        const map<wstring, CObj*>::iterator iter = m_mapObj.find(_strKey);
        if (iter != m_mapObj.end())
            return iter->second;
        return nullptr;
    }

    void Create(const wstring& _strKey, const Vec2& vPos);

    void SetSpawnMonster(const MONSTER _tMonster, const Vec2& _vPos)
    {
        m_strMon  = _tMonster;
        m_vMonPos = _vPos;
    }

    void         SpawnMonster(MONSTER _tMonster, const Vec2& m_vMonPos);
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CSpawnBox();
    CSpawnBox(const CSpawnBox& _other);
    virtual ~CSpawnBox() override;
};
