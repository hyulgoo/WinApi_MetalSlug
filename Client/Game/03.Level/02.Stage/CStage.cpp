#include "pch.h"
#include "Game\03.Level\02.Stage\CStage.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\01.KeyMgr\CKeyMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include "Game\04.Object\UI\CUI.h"

#include "Game\04.Object\BackGround\CBackGround.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\02.Manager\04.CollisionMgr\CCollisionMgr.h"
#include "Game\04.Object\BackGround\CBackGround_Back.h"
#include "Game\04.Object\BackGround\CBackGround_Front.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"

#include "Game\04.Object\Character\Monster\Enemy\CSoldier.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie.h"

#include "Game\04.Object\UI\PanelUI\CPanelUI.h"
#include "Game\04.Object\UI\NumberUI\CNumberUI.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

#include <algorithm>

namespace
{
    // 카메라가 경로를 따라 수직 이동할 때의 최대 속도 (배경 텍스처 픽셀 / 초)
    constexpr float CAMERA_PATH_SPEED = 300.f;

    // 월드 좌표로 위치를 가지는 오브젝트 레이어. 카메라를 따라 그린다
    // (예전에는 화면 좌표라 수평 이동은 9개 클래스가 카메라 속도로 각자 추정해 밀고, 수직 이동은 이 레이어들을 일괄로 옮겼다)
    constexpr LAYER WORLD_LAYERS[] =
    {
        LAYER::DEFAULT, LAYER::MONSTER, LAYER::PLAYER, LAYER::PLAYER_PROJECTILE, LAYER::PLAYER_FIREPROJECTILE,
        LAYER::CQC, LAYER::CQCAREA, LAYER::MONSTER_PROJECTILE, LAYER::ITEM, LAYER::EFFECT,
    };

    CObj* CreateMonster(const MONSTER _eType)
    {
        // 좀비는 종류별 데이터 하나로 (예전에는 다섯 클래스)
        if (MONSTER::SOLDIER == _eType || MONSTER::END == _eType)
            return new CSoldier;
        return new CZombie(_eType);
    }

    const wchar_t* MonsterToStr(const MONSTER _eType)
    {
        switch (_eType)
        {
        case MONSTER::SOLDIER: return L"SOLDIER";
        case MONSTER::ZB_GIRL: return L"ZB_GIRL";
        case MONSTER::ZB_BOY:  return L"ZB_BOY";
        case MONSTER::ZB_MAN:  return L"ZB_MAN";
        case MONSTER::ZB_OLD:  return L"ZB_OLD";
        case MONSTER::ZB_RSH:  return L"ZB_RSH";
        default:               return L"SOLDIER";
        }
    }

    MONSTER StrToMonster(const wchar_t* _szType)
    {
        if (!wcscmp(_szType, L"SOLDIER")) return MONSTER::SOLDIER;
        if (!wcscmp(_szType, L"ZB_GIRL")) return MONSTER::ZB_GIRL;
        if (!wcscmp(_szType, L"ZB_BOY"))  return MONSTER::ZB_BOY;
        if (!wcscmp(_szType, L"ZB_MAN"))  return MONSTER::ZB_MAN;
        if (!wcscmp(_szType, L"ZB_OLD"))  return MONSTER::ZB_OLD;
        if (!wcscmp(_szType, L"ZB_RSH"))  return MONSTER::ZB_RSH;
        return MONSTER::SOLDIER;
    }
}

CStage::CStage()
    : m_iSpawnIdx(0)
    , m_pTargetObj(nullptr)
    , m_vResolution{}
    , m_pPlayer(nullptr)
    , m_pBombCount1(nullptr)
    , m_pBombCount2(nullptr)
    , m_pBulletCount1(nullptr)
    , m_pBulletCount2(nullptr)
    , m_pBulletCount3(nullptr)
    , m_pLifeCount(nullptr)
    , m_pTimeCount1(nullptr)
    , m_pTimeCount2(nullptr)
    , m_iTime(59)
    , m_fTimeCount(0.f)
{
    for (const LAYER eLayer : WORLD_LAYERS)
        SetCameraLayer(eLayer);

    // 픽셀 충돌 맵(배경과 같은 텍스처 좌표). 예전에는 픽셀 충돌 컴포넌트가 이 스테이지의 텍스처를 직접 불렀다
    SetCollisionMap(CResMgr::GetInst()->LoadTexture(L"PixelBackGround", L"texture\\PixelBackGround.bmp"));
}

CStage::~CStage()
{
}

void CStage::Enter()
{
    // 에디터가 메뉴를 붙였다 떼면 창 크기가 바뀌므로 해상도를 다시 맞춘다
    const POINT ptResolution = CEngine::GetInst()->GetResolution();
    CEngine::GetInst()->ChangeWindowSize(ptResolution.x, ptResolution.y);

    // 진행 상태를 처음으로 돌린다
    m_iTime      = 59;
    m_fTimeCount = 0.f;

    AddObject(new CBackGround, LAYER::BACKGROUND);
    // 캐릭터 앞에 그리는 배경 조각 둘 (예전에는 두 번째를 Clone으로 만들고 복사 생성자가 다른 텍스처를 불렀다)
    AddObject(new CBackGround_Front(L"BackGround_Front", L"texture\\BackGround_Front01.bmp", Vec2(3658.f, 508.f)), LAYER::BACKGROUND_FRONT);
    AddObject(new CBackGround_Back, LAYER::BACKGROUND_BACK);
    AddObject(new CBackGround_Front(L"BackGround_Front2", L"texture\\BackGround_Front02.bmp", Vec2(5815.f, 322.f)), LAYER::BACKGROUND_FRONT);

    m_vResolution = CEngine::GetInst()->GetResolution();

    LoadMap(L"Stage01.level");

    // 카메라는 경로(배경 텍스처 좌표)의 시작 높이에서 출발한다.
    m_iSpawnIdx = 0;
    CCamera::GetInst()->SetLook(Vec2(m_vResolution.x / 2.f, m_vResolution.y / 2.f + GetCameraPathY(0.f) * BG_MAGNI));

    CreateUI();

    m_pPlayer = new CPlayer;
    m_pPlayer->SetPos(CCamera::GetInst()->GetRealPos(CPlayer::SPAWN_SCREEN_POS));
    m_pPlayer->SetSummon();
    AddObject(m_pPlayer, LAYER::PLAYER);

    m_pTargetObj = m_pPlayer;

    CCamera::GetInst()->FadeIn(3.f);

    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_FIREPROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::ITEM);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQC);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQCAREA);
}

void CStage::tick()
{
    CLevel::tick();

    UpdateCameraPath();
    SpawnMonster();

    // 남은 목숨 0에서 죽고 사망 연출이 끝나면 게임 오버 (예전에는 목숨이 0이 되는 순간 = 세 번째 사망 뒤 다시 나오는 순간이었다)
    if (m_pPlayer->IsGameOver())
        ChangeLevel(LEVEL_TYPE::GAMEOVER);

    // 화면 아래로 떨어지면 화면 높이 600에서 다시 소환한다
    if (m_pPlayer->GetScreenPos().y > 1500.f)
    {
        m_pPlayer->SetPos(Vec2(m_pPlayer->GetPos().x, CCamera::GetInst()->GetRealPos(Vec2(0.f, 600.f)).y));
        m_pPlayer->SetSummon();
    }

    RenewalUI();

    // 디버그 키 (Debug 빌드만)
    if constexpr (DEBUG_KEYS)
        TickDebugKeys();
}

void CStage::render(const HDC _dc)
{
   CLevel::render(_dc);

   if constexpr (DEBUG_KEYS)
       RenderDebugInfo(_dc);
}

void CStage::TickDebugKeys()
{
    if (IsTap(KEY::Q))
        SetRenderCollider();

    if (IsTap(KEY::R))
       m_pPlayer->GetPixelCollider()->ToggleShowMap();

    if(IsTap(KEY::ENTER))
        ChangeLevel(LEVEL_TYPE::ANIMEDITOR);

    if (IsPressed(KEY::A))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x - 300.f * DT, m_pTargetObj->GetPos().y));
    if (IsPressed(KEY::D))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x + 300.f * DT, m_pTargetObj->GetPos().y));
    if (IsPressed(KEY::S))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x, m_pTargetObj->GetPos().y - 300.f * DT));
    if (IsPressed(KEY::W))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x, m_pTargetObj->GetPos().y + 300.f * DT));
}

void CStage::RenderDebugInfo(const HDC _dc) const
{
   // 레벨 파일(카메라 경로, 몬스터 배치)과 같은 배경 텍스처 좌표로 표시
   const Vec2 vCmrLT = CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)) / BG_MAGNI;
   const Vec2 vRPos  = m_pTargetObj->GetPos() / BG_MAGNI;
   TCHAR Pos[64] = {};
   wsprintf(Pos, TEXT("X : %d  /  Y : %d  (CAM X : %d)"), static_cast<int>(vRPos.x), static_cast<int>(vRPos.y), static_cast<int>(vCmrLT.x));

   TextOut(_dc, static_cast<int>(m_vResolution.x - 300.f), 40, Pos, lstrlenW(Pos));    
}

void CStage::Exit()
{
    DeleteAllObjects();

    m_pPlayer       = nullptr;
    m_pTargetObj    = nullptr;
    m_pBombCount1   = nullptr;
    m_pBombCount2   = nullptr;
    m_pBulletCount1 = nullptr;
    m_pBulletCount2 = nullptr;
    m_pBulletCount3 = nullptr;
    m_pLifeCount    = nullptr;
    m_pTimeCount1   = nullptr;
    m_pTimeCount2   = nullptr;
}

void CStage::CreateUI()
{
    CTexture* pHPTex     = CResMgr::GetInst()->LoadTexture(L"HUD_HP", L"texture\\HUD_HP.bmp");
    CTexture* pWeaponTex = CResMgr::GetInst()->LoadTexture(L"HUD_WEAPONE", L"texture\\HUD_WEAPONE.bmp");

    // 목숨 (HP 패널)
    CPanelUI* pHPPanel = new CPanelUI;
    pHPPanel->SetIdleTex(pHPTex);
    pHPPanel->SetPos(Vec2(50.f, 80.f));

    m_pLifeCount = new CNumberUI;
    m_pLifeCount->SetScale(Vec2(50.f, 50.f));
    m_pLifeCount->SetPos(Vec2(180.f, 15.f));
    m_pLifeCount->SetPrintLife();

    // 폭탄 두 자리, 총알 세 자리, 남은 시간 두 자리 (무기 패널 그림 기준 위치)
    CPanelUI* pWeaponPanel = new CPanelUI;
    pWeaponPanel->SetIdleTex(pWeaponTex);
    pWeaponPanel->SetPos(Vec2(320.f, 40.f));

    const struct
    {
        CNumberUI* CStage::* pDigit = nullptr;
        float                fSize  = 0.f;
        Vec2                 vPos;
        bool                 bTime  = false;
    } arrDigit[] =
    {
        { &CStage::m_pBombCount1,   12.f, Vec2(168.f, 12.f), false },
        { &CStage::m_pBombCount2,   12.f, Vec2(200.f, 12.f), false },
        { &CStage::m_pBulletCount1, 10.f, Vec2(43.f, 12.f),  false },
        { &CStage::m_pBulletCount2, 10.f, Vec2(75.f, 12.f),  false },
        { &CStage::m_pBulletCount3, 10.f, Vec2(107.f, 12.f), false },
        { &CStage::m_pTimeCount1,   10.f, Vec2(260.f, -90.f), true },
        { &CStage::m_pTimeCount2,   10.f, Vec2(325.f, -90.f), true },
    };
    for (const auto& tDigit : arrDigit)
    {
        CNumberUI* pDigit = new CNumberUI;
        pDigit->SetScale(Vec2(tDigit.fSize, tDigit.fSize));
        pDigit->SetPos(tDigit.vPos);
        if (tDigit.bTime)
            pDigit->SetPrintTime();
        this->*tDigit.pDigit = pDigit;
    }

    pHPPanel->AddChildUI(m_pLifeCount);
    for (const auto& tDigit : arrDigit)
        pWeaponPanel->AddChildUI(this->*tDigit.pDigit);
    AddObject(pHPPanel, LAYER::UI);
    AddObject(pWeaponPanel, LAYER::UI);
}

void CStage::RenewalUI()
{
    if (m_pPlayer->IsHMG())
    {
        const int iBulletCount = m_pPlayer->GetBulletCount();
        m_pBulletCount1->SetNumber(iBulletCount / 100);
        m_pBulletCount2->SetNumber(iBulletCount % 100 / 10);
        m_pBulletCount3->SetNumber(iBulletCount % 10);
        if (m_pBulletCount2->IsNone())
            m_pBulletCount2->SetDone();
        if (m_pBulletCount3->IsNone())
            m_pBulletCount3->SetDone();
    }
    else if (!m_pPlayer->IsHMG())
    {
        m_pBulletCount1->SetNumber(10);
        m_pBulletCount2->SetNone();
        m_pBulletCount3->SetNone();
    }

    const int iBombCount = m_pPlayer->GetBombCount();
    m_pBombCount1->SetNumber(iBombCount / 10);
    m_pBombCount2->SetNumber(iBombCount % 10);

    const int iLifeCount = m_pPlayer->GetLifeCount();
    m_pLifeCount->SetNumber(iLifeCount);
    m_fTimeCount += DT;
    if (m_fTimeCount > 15.f)
    {
        --m_iTime;
        m_fTimeCount = 0.f;

        // 시간이 다 되면 원작처럼 플레이어가 죽고 시간이 다시 채워진다
        // (예전에는 하한이 없어 -1이 되면 숫자 표시가 GetFrmInfo()[-1]을 읽었다)
        if (m_iTime <= 0)
        {
            m_pPlayer->Kill();
            m_iTime = 59;
        }
    }
    m_pTimeCount1->SetNumber(m_iTime / 10);
    m_pTimeCount2->SetNumber(m_iTime % 10);
}

float CStage::GetCameraPathY(const float _fCameraX) const
{
    if (m_vecCmrPath.empty())
        return 0.f;

    if (_fCameraX <= m_vecCmrPath.front().x)
        return m_vecCmrPath.front().y;

    // 카메라 x 가 속한 구간을 찾아 선형 보간
    for (size_t i = 1; i < m_vecCmrPath.size(); ++i)
    {
        const Vec2& vPrev = m_vecCmrPath[i - 1];
        const Vec2& vNext = m_vecCmrPath[i];

        if (_fCameraX <= vNext.x)
        {
            const float fRatio = (_fCameraX - vPrev.x) / (vNext.x - vPrev.x);
            return vPrev.y + (vNext.y - vPrev.y) * fRatio;
        }
    }

    return m_vecCmrPath.back().y;
}

void CStage::UpdateCameraPath()
{
    if (m_vecCmrPath.empty())
        return;

    // 흔들림 오프셋을 제외한 카메라 좌상단 (배경 텍스처 좌표)
    const Vec2  vLook    = CCamera::GetInst()->GetLook();
    const float fCameraX = (vLook.x - m_vResolution.x / 2.f) / BG_MAGNI;
    const float fCurTop  = (vLook.y - m_vResolution.y / 2.f) / BG_MAGNI;

    // 수평 진행도에 맞는 경로상의 높이로 이동하되, 한 프레임 이동량을 제한해 튀지 않게 한다.
    const float fMaxMove = CAMERA_PATH_SPEED * DT;
    float       fMove    = GetCameraPathY(fCameraX) - fCurTop;
    if (fMove > fMaxMove)
        fMove = fMaxMove;
    else if (fMove < -fMaxMove)
        fMove = -fMaxMove;

    CCamera::GetInst()->SetLook(Vec2(vLook.x, vLook.y + fMove * BG_MAGNI));
}

void CStage::SpawnMonsterAt(const MONSTER _eType, const Vec2& _vScreenPos)
{
    Instantiate(CreateMonster(_eType), CCamera::GetInst()->GetRealPos(_vScreenPos), LAYER::MONSTER);
}

void CStage::SpawnMonster()
{
    // 자동 테스트가 nospawn 이면 레벨 파일의 배치를 쓰지 않는다
    if (CTestMgr::GetInst()->IsSpawnDisabled())
        return;

    // 카메라 왼쪽 위 (배경 텍스처 좌표)
    const Vec2 vCmrLT = CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)) / BG_MAGNI;

    // 등장 정보는 TriggerX 오름차순으로 정렬되어 있다.
    while (m_iSpawnIdx < m_vecSpawninfo.size() && m_vecSpawninfo[m_iSpawnIdx].TriggerX <= vCmrLT.x)
    {
        const MSTSPAWN& info = m_vecSpawninfo[m_iSpawnIdx++];

        // 배경 텍스처 좌표(발 위치) -> 월드 좌표(중심 위치)
        CObj* pMonster = CreateMonster(info.SpawnType);
        Vec2  vPos     = info.SpawnPos * BG_MAGNI;
        vPos.y         -= pMonster->GetScale().y / 2.f;
        Instantiate(pMonster, vPos, LAYER::MONSTER);
    }
}

void CStage::SaveMap(const wstring& _strRelativePath) const
{
    wstring strPath = CPathMgr::GetInst()->GetContentPath();
    strPath += L"level\\";
    strPath += _strRelativePath;

    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strPath.c_str(), L"w");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"레벨 저장 실패, Error Number : %d", iErrNum);
        ReportError(L"파일 저장 실패", szStr);
        return;
    }

    // 카메라 경로 저장
    fwprintf_s(pFile, L"[CAMERAPATH_COUNT]\n%zd\n\n", m_vecCmrPath.size());
    for (size_t i = 0; i < m_vecCmrPath.size(); ++i)
    {
        fwprintf_s(pFile, L"[%zd_CAMERAPATH]\n", i);
        fwprintf_s(pFile, L"[POS]\n%.1f %.1f\n\n", m_vecCmrPath[i].x, m_vecCmrPath[i].y);
    }

    // 몬스터 배치 저장
    fwprintf_s(pFile, L"[MONSTER_COUNT]\n%zd\n\n", m_vecSpawninfo.size());
    for (size_t i = 0; i < m_vecSpawninfo.size(); ++i)
    {
        const MSTSPAWN& info = m_vecSpawninfo[i];
        fwprintf_s(pFile, L"[%zd_MONSTER]\n", i);
        fwprintf_s(pFile, L"[TYPE]\n%s\n", MonsterToStr(info.SpawnType));
        fwprintf_s(pFile, L"[TRIGGER]\n%.1f\n", info.TriggerX);
        fwprintf_s(pFile, L"[POS]\n%.1f %.1f\n\n", info.SpawnPos.x, info.SpawnPos.y);
    }

    fclose(pFile);
}

void CStage::LoadMap(const wstring& _strRelativePath)
{
    m_vecCmrPath.clear();
    m_vecSpawninfo.clear();

    wstring strPath = CPathMgr::GetInst()->GetContentPath();
    strPath += L"level\\";
    strPath += _strRelativePath;

    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strPath.c_str(), L"r");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"레벨 로드 실패, Error Number : %d", iErrNum);
        ReportError(L"파일 로드 실패", szStr);
        return;
    }

    wchar_t szBuffer[256] = {};

    while (1 == fwscanf_s(pFile, L"%s", szBuffer, 256))
    {
        if (!wcscmp(szBuffer, L"[CAMERAPATH_COUNT]"))
        {
            size_t iCount = 0;
            fwscanf_s(pFile, L"%zd", &iCount);
            m_vecCmrPath.reserve(iCount);

            for (size_t i = 0; i < iCount; ++i)
            {
                Vec2 vPoint = {};

                while (1 == fwscanf_s(pFile, L"%s", szBuffer, 256))
                {
                    if (!wcscmp(szBuffer, L"[POS]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &vPoint.x, &vPoint.y);
                        break;
                    }
                }

                m_vecCmrPath.push_back(vPoint);
            }
        }
        else if (!wcscmp(szBuffer, L"[MONSTER_COUNT]"))
        {
            size_t iCount = 0;
            fwscanf_s(pFile, L"%zd", &iCount);
            m_vecSpawninfo.reserve(iCount);

            for (size_t i = 0; i < iCount; ++i)
            {
                MSTSPAWN info = {};

                while (1 == fwscanf_s(pFile, L"%s", szBuffer, 256))
                {
                    if (!wcscmp(szBuffer, L"[TYPE]"))
                    {
                        wchar_t szType[64] = {};
                        fwscanf_s(pFile, L"%s", szType, 64);
                        info.SpawnType = StrToMonster(szType);
                    }
                    else if (!wcscmp(szBuffer, L"[TRIGGER]"))
                        fwscanf_s(pFile, L"%f", &info.TriggerX);
                    else if (!wcscmp(szBuffer, L"[POS]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &info.SpawnPos.x, &info.SpawnPos.y);
                        break;
                    }
                }

                m_vecSpawninfo.push_back(info);
            }
        }
    }

    fclose(pFile);

    // 둘 다 카메라 x 진행도 순서로 처리하므로 x 기준 오름차순으로 정렬해 둔다.
    std::stable_sort(m_vecCmrPath.begin(), m_vecCmrPath.end()
                   , [](const Vec2& _vLeft, const Vec2& _vRight) { return _vLeft.x < _vRight.x; });
    std::stable_sort(m_vecSpawninfo.begin(), m_vecSpawninfo.end()
                   , [](const MSTSPAWN& _tLeft, const MSTSPAWN& _tRight) { return _tLeft.TriggerX < _tRight.TriggerX; });
}
