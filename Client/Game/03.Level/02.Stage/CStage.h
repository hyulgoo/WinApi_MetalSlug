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
        float fClearFadeWait   = 0.f;   // [clear] 보스를 이긴 뒤(승리 포즈부터) 화면이 어두워지기 시작할 때까지
        float fClearFade       = 0.f;   // 어두워지는 시간. 다 어두워지면 타이틀로
        float fDebugMoveSpeed  = 0.f;
        Vec2  vDebugInfoPos;
    };
    tData                  m_tData;

    // 보스를 이긴 뒤
    float                  m_fClearTime;
    bool                   m_bClearFade;
    bool                   m_bClearDone;

    bool                   m_bStartAtBoss;   // 다음에 들어올 때 보스전부터 (타이틀 F2). 들어오면서 한 번 쓰고 끈다

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
    void        UpdateClear();      // 보스를 이겼으면 잠시 뒤 화면을 어둡게 하고 타이틀로

    float       GetCameraPathY(float _fCameraX) const;
    void        UpdateCameraPath();
    void        SpawnMonster();
    void        JumpToBoss();       // 카메라를 레벨 파일의 BOSS 배치가 나오는 자리로 (앞의 배치는 건너뛴다)

public:
    void        SaveMap(const wstring& _strRelativePath) const;
    void        LoadMap(const wstring& _strRelativePath);

    // 다음에 들어올 때 처음이 아니라 보스가 나오는 자리에서 시작한다 (타이틀에서 F2)
    void        SetStartAtBoss() { m_bStartAtBoss = true; }

    // 자동 테스트용: 화면 좌표(중심)에 몬스터를 바로 등장시킨다
    void        SpawnMonsterAt(MONSTER _eType, const Vec2& _vScreenPos);
    // 자동 테스트용: 카메라 왼쪽을 배경 텍스처 x로 바로 옮긴다(높이는 카메라 경로). 플레이어는 화면 위치 그대로 따라오고,
    // 그 x보다 앞에서 나올 몬스터 배치는 건너뛴다
    void        JumpCamera(float _fCameraX);
    int         GetRemainTime() const { return m_iTime; }
    void        SetRemainTime(const int _iTime) { m_iTime = _iTime; }

public:
    CStage();
    virtual ~CStage() override;
};
