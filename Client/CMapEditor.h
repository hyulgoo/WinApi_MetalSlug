#pragma once
#include "CLevel.h"
#include "CObj.h"

class CTexture;
class CBackGround;
class CBackGround_Front;
class CPlayer;
class CButton;
class CCameraBox;

class CMapEditor :
    public CLevel
{
private:

    list<Vec2>            m_listCmrPos;
    list<Vec2>            m_listCmrDuration;
    map<wstring, CObj*>   m_mapObj;
    list<CCameraBox*>     m_listCmrBox;

    list<Vec2>            m_listSpawnPos;
    list<MONSTER>         m_listSpawnMonster;
    list<Vec2>            m_listSpawnBoxPos;
    int                   m_iMstCount;

    HMENU                 m_hMenu;
    CObj*                 m_pTargetObj;
    Vec2                  m_vResolution;
    float                 m_fTime;
    CPlayer*              m_pPlayer;
    CButton*              m_pBombCount1;
    CButton*              m_pBombCount2;
    CButton*              m_pBulletCount1;
    CButton*              m_pBulletCount2;
    CButton*              m_pBulletCount3;
    CButton*              m_pLifeCount;
    CButton*              m_pTimeCount1;
    CButton*              m_pTimeCount2;
    int                   m_iTime;
    float                 m_fTimeCount;
    CObj*                 m_pPlayerCQC;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Exit() override;
    virtual void Enter() override;
    void AddMonster(const wstring& _strKey, CObj* _pState)
    {
        m_mapObj.insert(make_pair(_strKey, _pState));
    }
    CObj* FindMonster(const wstring& _strKey)
    {
        map<wstring, CObj*>::iterator iter = m_mapObj.find(_strKey);
        if (iter != m_mapObj.end())
            return iter->second;
        return nullptr;
    }
    void Create(const wstring& _strKey, Vec2 vPos)
    {
       CObj* Monster = FindMonster(_strKey)->Clone();
       Instantiate(Monster, vPos, LAYER::MONSTER);
    }
    void addMstCount() { ++m_iMstCount; }
private:
    void CreateUI();
    void RenewalUI();
    void CreateCameraBox();
    void CreateSpawnBox();

public:
   // void SaveMap();
   // void LoadMap();
   // void PrintEditObject(HDC _dc, int _iLayerNumber, int _iObjectNumber);

public:
    CMapEditor();
    ~CMapEditor();
};

