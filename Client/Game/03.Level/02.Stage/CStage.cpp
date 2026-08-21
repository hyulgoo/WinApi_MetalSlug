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

#include "Game\04.Object\EventBox\CCameraBox.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"
#include "Game\04.Object\EventBox\CSpawnBox.h"

#include "Game\04.Object\UI\PanelUI\CPanelUI.h"
#include "Game\04.Object\UI\ButtonUI\CButton.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"

namespace
{
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
    : m_hMenu(nullptr)
    , m_pTargetObj(nullptr)
    , m_vResolution{}
    , m_fTime(0.f)
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
    , m_pPlayerCQC(nullptr)
{
}

CStage::~CStage()
{
}

void CStage::init()
{
    CBackGround* pCBackGround = new CBackGround;
    AddObject(pCBackGround, LAYER::BACKGROUND);    
    CBackGround_Front* pCBackGround_Front = new CBackGround_Front;
    AddObject(pCBackGround_Front, LAYER::BACKGROUND_FRONT);    
    CBackGround_Back* pBackGround_Back = new CBackGround_Back;
    AddObject(pBackGround_Back, LAYER::BACKGROUND_BACK);    
    CBackGround_Front* pCBackGround_Front2 = pCBackGround_Front->Clone();
    AddObject(pCBackGround_Front2, LAYER::BACKGROUND_FRONT);
    
    m_vResolution = CEngine::GetInst()->GetResolution();
    CCamera::GetInst()->SetLook(Vec2(m_vResolution.x / 2.f, m_vResolution.y / 2.f + 45));

    LoadMap(L"Stage01.level");

    CreateUI();
    CreateCameraBox();
    CreateSpawnBox();

    m_pPlayer = new CPlayer;
    m_pPlayer->SetPos(Vec2(100.f, 500.f));
    m_pPlayer->SetSumon();
    AddObject(m_pPlayer, LAYER::PLAYER);

    m_pTargetObj = m_pPlayer;

    CCamera::GetInst()->SetCameraSpeed(m_pPlayer->GetSpeed());
    CCamera::GetInst()->FadeIn(3.f);

    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_FIREPROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::EDITUI);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::ITEM);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQC);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQCAREA);
}

void CStage::tick()
{
    CLevel::tick();

    if (m_pPlayer->GetLifeCount() < 1)
        ChangeLevel(LEVEL_TYPE::GAMEOVER);

    if (m_pPlayer->GetPos().y > 1500.f)
    {
        m_pPlayer->SetPos(Vec2(m_pPlayer->GetPos().x, 600.f));
        m_pPlayer->SetSumon();
    }

    RenewalUI();

    if (IsTap(KEY::Q))
        SetRenderCollider();

    if (IsTap(KEY::R))
       m_pPlayer->GetPixelCollider()->SetPlayPixel();

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

void CStage::render(const HDC _dc)
{
   CLevel::render(_dc);

   const Vec2 vRPos = CCamera::GetInst()->GetRealPos(m_pTargetObj->GetPos());
   TCHAR Pos[32] = {};
   wsprintf(Pos, TEXT("X : %d  /  Y : %d"), static_cast<int>(vRPos.x), static_cast<int>(vRPos.y));
   
   TextOut(_dc, static_cast<int>(m_vResolution.x - 300.f), 40, Pos, lstrlenW(Pos));    
}

void CStage::Enter()
{
    const POINT ptResolution = CEngine::GetInst()->GetResolution();
    CEngine::GetInst()->ChangeWindowSize(ptResolution.x, ptResolution.y);

    init();
}

void CStage::Exit()
{
    DeleteObject();
}

void CStage::CreateUI()
{
    CTexture* pHPTex = CResMgr::GetInst()->LoadTexture(L"HUD_HP", L"texture\\HUD_HP.bmp");
    CTexture* pWeaPoneTex = CResMgr::GetInst()->LoadTexture(L"HUD_WEAPONE", L"texture\\HUD_WEAPONE.bmp");

    CPanelUI* pHPPanel = new CPanelUI;
    pHPPanel->SetIdleTex(pHPTex);
    pHPPanel->SetPos(Vec2(50.f, 80.f));

    m_pLifeCount = new CButton;
    m_pLifeCount->SetScale(Vec2(50.f, 50.f));
    m_pLifeCount->SetPos(Vec2(180.f, 15.f));
    m_pLifeCount->SetPrintLife();

    CPanelUI* pWeaponePanel = new CPanelUI;
    pWeaponePanel->SetIdleTex(pWeaPoneTex);
    pWeaponePanel->SetPos(Vec2(320.f, 40.f));

    m_pBombCount1 = new CButton;
    m_pBombCount1->SetScale(Vec2(12.f, 12.f));
    m_pBombCount1->SetPos(Vec2(168.f, 12.f));
    
    m_pBombCount2 = new CButton;
    m_pBombCount2->SetScale(Vec2(12.f, 12.f));
    m_pBombCount2->SetPos(Vec2(200.f, 12.f));
    
    m_pBulletCount1 = new CButton;
    m_pBulletCount1->SetScale(Vec2(10.f, 10.f));
    m_pBulletCount1->SetPos(Vec2(43.f, 12.f));
    
    m_pBulletCount2 = new CButton;
    m_pBulletCount2->SetScale(Vec2(10.f, 10.f));
    m_pBulletCount2->SetPos(Vec2(75.f, 12.f));
    
    m_pBulletCount3 = new CButton;
    m_pBulletCount3->SetScale(Vec2(10.f, 10.f));
    m_pBulletCount3->SetPos(Vec2(107.f, 12.f));

    m_pTimeCount1 = new CButton;
    m_pTimeCount1->SetScale(Vec2(10.f, 10.f));
    m_pTimeCount1->SetPos(Vec2(260.f, -90.f));
    m_pTimeCount1->SetPrintTime();

    m_pTimeCount2 = new CButton;
    m_pTimeCount2->SetScale(Vec2(10.f, 10.f));
    m_pTimeCount2->SetPos(Vec2(325.f, -90.f));
    m_pTimeCount2->SetPrintTime();

    pHPPanel->AddChildUI(m_pLifeCount);
    pWeaponePanel->AddChildUI(m_pBombCount1);
    pWeaponePanel->AddChildUI(m_pBombCount2);
    pWeaponePanel->AddChildUI(m_pBulletCount1);
    pWeaponePanel->AddChildUI(m_pBulletCount2);
    pWeaponePanel->AddChildUI(m_pBulletCount3);
    pWeaponePanel->AddChildUI(m_pTimeCount1);
    pWeaponePanel->AddChildUI(m_pTimeCount2);
    AddObject(pHPPanel, LAYER::UI);
    AddObject(pWeaponePanel, LAYER::UI);
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
    }
    m_pTimeCount1->SetNumber(m_iTime / 10);
    m_pTimeCount2->SetNumber(m_iTime % 10);
}

void CStage::CreateCameraBox()
{
    for (const CMRSPAWN& info : m_vecCmrSpawninfo)
    {
        CCameraBox* Box = new CCameraBox;
        Box->SetPos(info.SpawnPos);
        Box->SetMove(info.UpDown, info.Duration.x, info.Duration.y);
        AddObject(Box, LAYER::EDITUI);
        m_queueCmrBox.push(Box);
    }
}

void CStage::CreateSpawnBox()
{
    for (const MSTSPAWN& info : m_vecSpawninfo)
    {
        CSpawnBox* Box = new CSpawnBox;
        Box->SetSpawnMonster(info.SpawnType, info.SpawnPos);
        Instantiate(Box, info.SpawnBoxPos, LAYER::EDITUI);
    }
}

void CStage::SaveMap(const wstring& _strRelativePath) const
{
    wstring strPath = CPathMgr::GetInst()->GetContentPath();
    strPath += L"level\\";
    strPath += _strRelativePath;

    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strPath.c_str(), L"wb");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"레벨 저장 실패, Error Number : %d", iErrNum);
        MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
        return;
    }

    // 카메라박스 배치 저장
    fwprintf_s(pFile, L"[CAMERABOX_COUNT]\n%zd\n\n", m_vecCmrSpawninfo.size());
    for (size_t i = 0; i < m_vecCmrSpawninfo.size(); ++i)
    {
        const CMRSPAWN& info = m_vecCmrSpawninfo[i];
        fwprintf_s(pFile, L"[%zd_CAMERABOX]\n", i);
        fwprintf_s(pFile, L"[POS]\n%.1f %.1f\n", info.SpawnPos.x, info.SpawnPos.y);
        fwprintf_s(pFile, L"[UPDOWN]\n%d\n", info.UpDown ? 1 : 0);
        fwprintf_s(pFile, L"[DURATION]\n%.4f %.4f\n\n", info.Duration.x, info.Duration.y);
    }

    // 몬스터 배치 저장
    fwprintf_s(pFile, L"[MONSTER_COUNT]\n%zd\n\n", m_vecSpawninfo.size());
    for (size_t i = 0; i < m_vecSpawninfo.size(); ++i)
    {
        const MSTSPAWN& info = m_vecSpawninfo[i];
        fwprintf_s(pFile, L"[%zd_MONSTER]\n", i);
        fwprintf_s(pFile, L"[TYPE]\n%s\n", MonsterToStr(info.SpawnType));
        fwprintf_s(pFile, L"[VELOCITY]\n%.1f %.1f\n", info.SpawnPos.x, info.SpawnPos.y);
        fwprintf_s(pFile, L"[POS]\n%.1f %.1f\n\n", info.SpawnBoxPos.x, info.SpawnBoxPos.y);
    }

    fclose(pFile);
}

void CStage::LoadMap(const wstring& _strRelativePath)
{
    m_vecCmrSpawninfo.clear();
    m_vecSpawninfo.clear();

    wstring strPath = CPathMgr::GetInst()->GetContentPath();
    strPath += L"level\\";
    strPath += _strRelativePath;

    FILE*         pFile   = nullptr;
    const errno_t iErrNum = _wfopen_s(&pFile, strPath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        wchar_t szStr[256] = {};
        wsprintf(szStr, L"레벨 로드 실패, Error Number : %d", iErrNum);
        MessageBox(nullptr, szStr, L"파일 로드 실패", MB_OK);
        return;
    }

    wchar_t szBuffer[256] = {};

    while (1 == fwscanf_s(pFile, L"%s", szBuffer, 256))
    {
        if (!wcscmp(szBuffer, L"[CAMERABOX_COUNT]"))
        {
            size_t iCount = 0;
            fwscanf_s(pFile, L"%zd", &iCount);
            m_vecCmrSpawninfo.reserve(iCount);

            for (size_t i = 0; i < iCount; ++i)
            {
                CMRSPAWN info = {};

                while (true)
                {
                    fwscanf_s(pFile, L"%s", szBuffer, 256);

                    if (!wcscmp(szBuffer, L"[POS]"))
                        fwscanf_s(pFile, L"%f %f", &info.SpawnPos.x, &info.SpawnPos.y);
                    else if (!wcscmp(szBuffer, L"[UPDOWN]"))
                    {
                        int iUpDown = 0;
                        fwscanf_s(pFile, L"%d", &iUpDown);
                        info.UpDown = (0 != iUpDown);
                    }
                    else if (!wcscmp(szBuffer, L"[DURATION]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &info.Duration.x, &info.Duration.y);
                        break;
                    }
                }

                m_vecCmrSpawninfo.push_back(info);
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

                while (true)
                {
                    fwscanf_s(pFile, L"%s", szBuffer, 256);

                    if (!wcscmp(szBuffer, L"[TYPE]"))
                    {
                        wchar_t szType[64] = {};
                        fwscanf_s(pFile, L"%s", szType, 64);
                        info.SpawnType = StrToMonster(szType);
                    }
                    else if (!wcscmp(szBuffer, L"[VELOCITY]"))
                        fwscanf_s(pFile, L"%f %f", &info.SpawnPos.x, &info.SpawnPos.y);
                    else if (!wcscmp(szBuffer, L"[POS]"))
                    {
                        fwscanf_s(pFile, L"%f %f", &info.SpawnBoxPos.x, &info.SpawnBoxPos.y);
                        break;
                    }
                }

                m_vecSpawninfo.push_back(info);
            }
        }
    }

    fclose(pFile);
}
