#pragma once
#include "Game\03.Level\CLevel.h"

class CTexture;
class CBackGround;
class CBackGround_Front;
class CPlayer;
class CNumberUI;

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
    // 수치 (data\stage.ini, 디버그 키는 game.ini [debug]). 들어올 때마다 읽는다
    struct tData
    {
        int   iTime            = 0;     // 남은 시간을 채우는 값
        float fTimeTick        = 0.f;   // 남은 시간을 1 줄이는 간격
        float fFallY           = 0.f;   // 이 화면 높이보다 떨어지면 다시 소환
        float fFallRespawnY    = 0.f;
        float fFadeIn          = 0.f;
        float fCameraPathSpeed = 0.f;   // 배경 텍스처 px/초
        int   iInfiniteFrame   = 0;     // 권총 총알 수 자리에 그리는 숫자 그림
        float fDebugMoveSpeed  = 0.f;
        Vec2  vDebugInfoPos;
    };
    tData                  m_tData;

    // 카메라 경로 : 카메라 좌측 x 에 따른 카메라 상단 y (배경 텍스처 픽셀 기준, x 오름차순)
    vector<Vec2>           m_vecCmrPath;

    vector<MSTSPAWN>       m_vecSpawninfo;
    size_t                 m_iSpawnIdx;

    CObj*                 m_pTargetObj;
    Vec2                  m_vResolution;
    CPlayer*              m_pPlayer;
    CNumberUI*              m_pBombCount1;
    CNumberUI*              m_pBombCount2;
    CNumberUI*              m_pBulletCount1;
    CNumberUI*              m_pBulletCount2;
    CNumberUI*              m_pBulletCount3;
    CNumberUI*              m_pLifeCount;
    CNumberUI*              m_pTimeCount1;
    CNumberUI*              m_pTimeCount2;
    int                   m_iTime;
    float                 m_fTimeCount;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Exit() override;
    virtual void Enter() override;

private:
    void        CreateUI();
    void        TickDebugKeys();
    void        RenderDebugInfo(HDC _dc) const;
    void        RenewalUI();

    float       GetCameraPathY(float _fCameraX) const;
    void        UpdateCameraPath();
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
