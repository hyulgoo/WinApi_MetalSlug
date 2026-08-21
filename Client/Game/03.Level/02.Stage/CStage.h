#pragma once
#include "Game\03.Level\CLevel.h"

class CTexture;
class CBackGround;
class CBackGround_Front;
class CPlayer;
class CButton;
class CCameraBox;
class CSpawnBox;

struct CMRSPAWN
{
    Vec2    SpawnPos;
    bool    UpDown;
    Vec2    Duration;
};
struct MSTSPAWN
{
    MONSTER SpawnType;
    Vec2    SpawnPos;
    Vec2    SpawnBoxPos;
};

class CStage :
    public CLevel
{
private:

    vector<CMRSPAWN>       m_vecCmrSpawninfo;
    queue<CCameraBox*>     m_queueCmrBox;

    vector<MSTSPAWN>       m_vecSpawninfo;

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

private:
    void        CreateUI();
    void        RenewalUI();
    void        CreateCameraBox();
    void        CreateSpawnBox();

public:
    void        SaveMap(const wstring& _strRelativePath) const;
    void        LoadMap(const wstring& _strRelativePath);

public:
    CStage();
    virtual ~CStage() override;
};