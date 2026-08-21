#include "pch.h"
#include "CStage.h"

#include "CEngine.h"
#include "CCamera.h"
#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CLevelMgr.h"
#include "CPathMgr.h"
#include "CTexture.h"

#include "CUI.h"

#include "resource.h"
#include "CBackGround.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CBackGround_Back.h"
#include "CBackGround_Front.h"
#include "CTimeMgr.h"
#include "CMonster.h"
#include "CSoldier.h"
#include "CZombie_RSH.h"
#include "CZombie_oldman.h"
#include "CZombie_girl.h"
#include "CZombie_man.h"
#include "CZombie_boy.h"

#include "CCameraBox.h"
#include "CPixelCollider.h"
#include "CSpawnBox.h"

#include "CPanelUI.h"
#include "CButton.h"

CStage::CStage()
    : m_hMenu(nullptr)
    , m_vResolution{}
    , m_pTargetObj(nullptr)
    , m_fTime(0.f)
    , m_pPlayer(nullptr)
    , m_iTime(59)
    , m_fTimeCount(0.f)
    , m_pBombCount1(nullptr)
    , m_pBombCount2(nullptr)
    , m_pBulletCount1(nullptr)
    , m_pBulletCount2(nullptr)
    , m_pBulletCount3(nullptr)
    , m_pLifeCount(nullptr)
    , m_pTimeCount1(nullptr)
    , m_pTimeCount2(nullptr)
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

void CStage::render(HDC _dc)
{
   CLevel::render(_dc);

   Vec2 vRPos = CCamera::GetInst()->GetRealPos(m_pTargetObj->GetPos());
   TCHAR Pos[32] = {};
   wsprintf(Pos, TEXT("X : %d  /  Y : %d"), (int)(vRPos.x), (int)(vRPos.y));
   
   TextOut(_dc, (int)(m_vResolution.x - 300.f), 40, Pos, lstrlenW(Pos));    
}

void CStage::Enter()
{
    POINT ptResolution = CEngine::GetInst()->GetResolution();
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
        int iBulletCount = m_pPlayer->GetBulletCount();
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

    int iBombCount = m_pPlayer->GetBombCount();
    m_pBombCount1->SetNumber(iBombCount / 10);
    m_pBombCount2->SetNumber(iBombCount % 10);

    int iLifeCount = m_pPlayer->GetLifeCount();
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
    m_queueCmrSpawninfo.push({ Vec2(1600.f + 410.f, 500.f), Vec2(0.96f,   0.5f)});
    m_queueCmrSpawninfo.push({ Vec2(2370.f + 410.f, 500.f), Vec2(0.46f,   0.5f)});
    m_queueCmrSpawninfo.push({ Vec2(2570.f + 410.f, 500.f), Vec2(0.9f,    0.5f)});
    m_queueCmrSpawninfo.push({ Vec2(2700.f + 410.f, 500.f), Vec2(0.54f,   0.5f)});
    m_queueCmrSpawninfo.push({ Vec2(2825.f + 410.f, 500.f), Vec2(0.52f,   1.5f)});
    m_queueCmrSpawninfo.push({ Vec2(3100.f + 410.f, 500.f), Vec2(0.45f,   0.4f)});
    m_queueCmrSpawninfo.push({ Vec2(3180.f + 410.f, 500.f), Vec2(0.7056f, 1.8f)});
    m_queueCmrSpawninfo.push({ Vec2(3270.f + 410.f, 500.f), Vec2(0.425f,  0.4f)});
    m_queueCmrSpawninfo.push({ Vec2(3450.f + 410.f, 500.f), Vec2(0.54f,   1.5f)});
    m_queueCmrSpawninfo.push({ Vec2(3515.f + 410.f, 500.f), Vec2(0.9f,    0.5f)});
    m_queueCmrSpawninfo.push({ Vec2(3670.f + 410.f, 500.f), Vec2(0.4f,    0.4f)});
    m_queueCmrSpawninfo.push({ Vec2(4820.f + 410.f, 500.f), Vec2(0.3f,    0.3f)});
    m_queueCmrSpawninfo.push({ Vec2(4970.f + 410.f, 500.f), Vec2(0.475f,  0.4f)});
    m_queueCmrSpawninfo.push({ Vec2(5105.f + 410.f, 500.f), Vec2(0.425f,  0.4f)});
    m_queueCmrSpawninfo.push({ Vec2(5325.f + 410.f, 500.f), Vec2(0.475f,  0.4f)});

    for (int i = 0; i < 15; ++i)
    {
        CCameraBox* Box = new CCameraBox;
        Box->SetPos(m_queueCmrSpawninfo.front().SpawnPos);
        bool updown = false;
        if (i < 1)
            updown = true;
        Box->SetMove(updown, m_queueCmrSpawninfo.front().Duration.x, m_queueCmrSpawninfo.front().Duration.y);
        AddObject(Box, LAYER::EDITUI);
        m_queueCmrSpawninfo.pop();
        m_queueCmrBox.push(Box); 
    }
}

void CStage::CreateSpawnBox()
{
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(300.f, 300.f)});
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(1050.f, 300.f)});
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(1150.f, 300.f)});
    m_queueSpawninfo.push({ MONSTER::ZB_OLD , Vec2(0.f, 300.f), Vec2(1170.f, 300.f)});
    m_queueSpawninfo.push({ MONSTER::ZB_OLD , Vec2(0.f, 300.f), Vec2(1210.f, 300.f)});

    m_queueSpawninfo.push({ MONSTER::ZB_OLD,  Vec2(0.f, 300.f), Vec2(1570.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH,  Vec2(0.f, 300.f), Vec2(1620.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH,  Vec2(0.f, 300.f), Vec2(1640.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(1670.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY,  Vec2(0.f, 300.f), Vec2(1780.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH,  Vec2(0.f, 300.f), Vec2(1910.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(1980.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2000.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2020.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2030.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY,  Vec2(0.f, 300.f), Vec2(2070.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2200.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2220 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2230 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL, Vec2(0.f, 300.f), Vec2(2270 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2350 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2370 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2390 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2410 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2420 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2460 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2490 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2500 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2510 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2560 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2590 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2600 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2630 + 100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::SOLDIER, Vec2(0.f, 300.f), Vec2(2650 + 100.f, 300.f) });

    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f), Vec2(3050.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f), Vec2(3100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f), Vec2(3200.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f), Vec2(3300.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f), Vec2(3400.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f), Vec2(3500.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f), Vec2(3600.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f), Vec2(3700.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f), Vec2(3800.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f), Vec2(3900.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f), Vec2(4000.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f), Vec2(4100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f), Vec2(4200.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f), Vec2(4300.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f), Vec2(4400.f, 300.f) });

    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f) ,Vec2(4550.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f) ,Vec2(4600.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f) ,Vec2(4700.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f) ,Vec2(4800.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f) ,Vec2(4900.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f) ,Vec2(5000.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f) ,Vec2(5100.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f) ,Vec2(5200.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f) ,Vec2(5300.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f) ,Vec2(5400.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_MAN ,Vec2(0.f, 300.f) ,Vec2(5500.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_RSH ,Vec2(0.f, 300.f) ,Vec2(5600.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_GIRL,Vec2(0.f, 300.f) ,Vec2(5700.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_BOY ,Vec2(0.f, 300.f) ,Vec2(5800.f, 300.f) });
    m_queueSpawninfo.push({ MONSTER::ZB_OLD ,Vec2(0.f, 300.f) ,Vec2(5900.f, 300.f) });
        
    while (!m_queueSpawninfo.empty())
    {
        CSpawnBox* Box = new CSpawnBox;
        Box->SetSpawnMonster(m_queueSpawninfo.front().SpawnType, m_queueSpawninfo.front().SpawnPos);
        Instantiate(Box, m_queueSpawninfo.front().SpawnBoxPos, LAYER::EDITUI);
        m_queueSpawninfo.pop();
    }
}
