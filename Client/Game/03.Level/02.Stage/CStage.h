#pragma once
#include "Game\03.Level\CLevel.h"

class CTexture;
class CBackGround;
class CBackGround_Front;
class CPlayer;
class CButton;

// 몬스터 등장 정보 (좌표는 모두 배경 텍스처 픽셀 기준)
struct MSTSPAWN
{
    MONSTER SpawnType;
    float   TriggerX; // 카메라 좌측 x가 이 값에 도달하면 등장
    Vec2    SpawnPos; // 등장 위치 (발 위치 기준)
};

class CStage :
    public CLevel
{
private:
    // 카메라 경로 : 카메라 좌측 x 에 따른 카메라 상단 y (배경 텍스처 픽셀 기준, x 오름차순)
    vector<Vec2>           m_vecCmrPath;
    float                  m_fPrevCmrTop;

    vector<MSTSPAWN>       m_vecSpawninfo;
    size_t                 m_iSpawnIdx;

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
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Exit() override;
    virtual void Enter() override;

private:
    void        CreateUI();
    void        RenewalUI();

    float       GetCameraPathY(float _fCameraX) const;
    void        UpdateCameraPath();
    void        MoveWorldWithCamera();
    void        SpawnMonster();

public:
    void        SaveMap(const wstring& _strRelativePath) const;
    void        LoadMap(const wstring& _strRelativePath);

    // 자동 테스트용: 화면 좌표(중심)에 몬스터를 바로 등장시킨다
    void        SpawnMonsterAt(MONSTER _eType, const Vec2& _vScreenPos);
    int         GetRemainTime() const { return m_iTime; }
    void        SetRemainTime(const int _iTime) { m_iTime = _iTime; }

public:
    CStage();
    virtual ~CStage() override;
};
