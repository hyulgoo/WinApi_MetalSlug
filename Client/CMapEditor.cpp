#include "pch.h"
#include "CMapEditor.h"

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
#include "CCameraBox.h"
#include "CSpawnBox.h"

#include "CPanelUI.h"
#include "CButton.h"

CMapEditor::CMapEditor()
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
{
    m_mapObj.clear();
    AddMonster(L"Soldier", new CSoldier);
    AddMonster(L"Zombie_boy", new CZombie_boy);
    AddMonster(L"Zombie_man", new CZombie_man);
    AddMonster(L"Zombie_oldman", new CZombie_oldman);
    AddMonster(L"Zombie_RSH", new CZombie_RSH);
    AddMonster(L"Zombie_girl", new CZombie_girl);
}

CMapEditor::~CMapEditor()
{
    if (m_hMenu != nullptr)
        DestroyMenu(m_hMenu);
    map<wstring, CObj*>::iterator iter = m_mapObj.begin();
    for (; iter != m_mapObj.end(); ++iter)
    {
        DEL(iter->second);
    }
    m_mapObj.clear();
}

void CMapEditor::init()
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

    m_pPlayer = new CPlayer;
    m_pPlayer->SetPos(Vec2(200.f, 500.f));
    AddObject(m_pPlayer, LAYER::PLAYER);

    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_FIREPROJECTILE, LAYER::MONSTER);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER_PROJECTILE, LAYER::MONSTER_PROJECTILE);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::PLAYER, LAYER::EDITUI);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQC);
    CCollisionMgr::GetInst()->LayerCheck(LAYER::MONSTER, LAYER::CQCAREA);

    m_pTargetObj = m_pPlayer;

    CCamera::GetInst()->SetCameraSpeed(330);

    CreateUI();

    CreateCameraBox();

    CreateSpawnBox();

    CCamera::GetInst()->FadeIn(2.f);
}

void CMapEditor::tick()
{
    //if (m_pCBox->IsDead())
    RenewalUI();
    if (IsTap(KEY::R))
    {
       m_pPlayer->GetPixelCollider()->SetPlayPixel();
    }

    if(IsTap(KEY::ENTER))   
    ChangeLevel(LEVEL_TYPE::ANIMEDITOR);
     CLevel::tick();
    if (IsPressed(KEY::A))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x - 300.f * DT, m_pTargetObj->GetPos().y));
    if (IsPressed(KEY::D))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x + 300.f * DT, m_pTargetObj->GetPos().y));
    if (IsPressed(KEY::S))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x, m_pTargetObj->GetPos().y - 300.f * DT));
    if (IsPressed(KEY::W))
        m_pTargetObj->SetPos(Vec2(m_pTargetObj->GetPos().x, m_pTargetObj->GetPos().y + 300.f * DT));

}

void CMapEditor::render(HDC _dc)
{
    CLevel::render(_dc);
   Vec2 vRPos = CCamera::GetInst()->GetRealPos(m_pTargetObj->GetPos());
   TCHAR Pos[32] = {};
   wsprintf(Pos, TEXT("X : %d  /  Y : %d"), (int)(vRPos.x), (int)(vRPos.y));
   
   TextOut(_dc, (int)(m_vResolution.x - 300.f), 40, Pos, lstrlenW(Pos));
   //PrintEditObject(_dc, m_iLayerNumber, m_iObjectNumber);
    
}

void CMapEditor::Enter()
{
    // 에디터레벨에 들어올 때 메뉴바 추가
    if (m_hMenu == nullptr)
        m_hMenu = LoadMenu(nullptr, MAKEINTRESOURCEW(IDC_CLIENT));

    // 메뉴바 추가로 인한 해상도 재설정
    HWND hWnd = CEngine::GetInst()->GetMainWnd();
    SetMenu(hWnd, m_hMenu);

    POINT ptResolution = CEngine::GetInst()->GetResolution();
    CEngine::GetInst()->ChangeWindowSize(ptResolution.x, ptResolution.y);

    init();
}

void CMapEditor::Exit()
{
    HWND hWnd = CEngine::GetInst()->GetMainWnd();
    SetMenu(hWnd, nullptr);

    POINT ptResolution = CEngine::GetInst()->GetResolution();
    CEngine::GetInst()->ChangeWindowSize(ptResolution.x, ptResolution.y);

    DeleteObject();
}

void CMapEditor::CreateUI()
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

void CMapEditor::RenewalUI()
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

void CMapEditor::CreateCameraBox()
{
    m_listCmrPos.clear();
    m_listCmrDuration.clear();
    m_listCmrPos.push_back(Vec2(1600.f, 500.f));
    m_listCmrPos.push_back(Vec2(2370.f, 500.f));
    m_listCmrPos.push_back(Vec2(2570.f, 500.f));
    m_listCmrPos.push_back(Vec2(2700.f, 500.f));
    m_listCmrPos.push_back(Vec2(2825.f, 500.f));
    m_listCmrPos.push_back(Vec2(3100.f, 500.f));
    m_listCmrPos.push_back(Vec2(3180.f, 500.f));
    m_listCmrPos.push_back(Vec2(3270.f, 500.f));
    m_listCmrPos.push_back(Vec2(3450.f, 500.f));
    m_listCmrPos.push_back(Vec2(3515.f, 500.f));
    m_listCmrPos.push_back(Vec2(3670.f, 500.f));
    m_listCmrPos.push_back(Vec2(4820.f, 500.f));
    m_listCmrPos.push_back(Vec2(4970.f, 500.f));
    m_listCmrPos.push_back(Vec2(5105.f, 500.f));
    m_listCmrPos.push_back(Vec2(5325.f, 500.f));

    m_listCmrDuration.push_back(Vec2(0.96f, 0.5f));
    m_listCmrDuration.push_back(Vec2(0.46f,0.5f));
    m_listCmrDuration.push_back(Vec2(0.9f,0.5f));
    m_listCmrDuration.push_back(Vec2(0.54f, 0.5f));
    m_listCmrDuration.push_back(Vec2(0.52f, 1.5f));
    m_listCmrDuration.push_back(Vec2(0.45f, 0.4f));
    m_listCmrDuration.push_back(Vec2(0.7056f, 1.8f));
    m_listCmrDuration.push_back(Vec2(0.425f, 0.4f));
    m_listCmrDuration.push_back(Vec2(0.54f, 1.5f));
    m_listCmrDuration.push_back(Vec2(0.9f, 0.5f));
    m_listCmrDuration.push_back(Vec2(0.4f, 0.4f));
    m_listCmrDuration.push_back(Vec2(0.3f, 0.3f));
    m_listCmrDuration.push_back(Vec2(0.475f, 0.4f));
    m_listCmrDuration.push_back(Vec2(0.425f, 0.4f));
    m_listCmrDuration.push_back(Vec2(0.475f, 0.4f));
    m_listCmrBox.clear();

    for (int i = 0; i < 15; ++i)
    {
        CCameraBox* Box = new CCameraBox;
        Box->SetPos(m_listCmrPos.front());
        bool updown = false;
        if (i < 1)
            updown = true;
        Box->SetMove(updown, m_listCmrDuration.front().x, m_listCmrDuration.front().y);
        AddObject(Box, LAYER::EDITUI);
        m_listCmrPos.pop_front();
        m_listCmrDuration.pop_front();
        m_listCmrBox.push_back(Box); 
    }
}

void CMapEditor::CreateSpawnBox()
{
    m_listSpawnMonster.clear();
    m_listSpawnPos.clear();
    m_listSpawnBoxPos.clear();

    {
        m_listSpawnMonster.push_back(MONSTER::SOLDIER);
        m_listSpawnMonster.push_back(MONSTER::ZB_RSH);
        m_listSpawnMonster.push_back(MONSTER::ZB_MAN);
        m_listSpawnMonster.push_back(MONSTER::ZB_RSH);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_BOY);
        m_listSpawnMonster.push_back(MONSTER::ZB_BOY);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_OLD);
        m_listSpawnMonster.push_back(MONSTER::ZB_OLD);
        m_listSpawnMonster.push_back(MONSTER::ZB_OLD);
        m_listSpawnMonster.push_back(MONSTER::ZB_RSH);
        m_listSpawnMonster.push_back(MONSTER::ZB_RSH);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_BOY);
        m_listSpawnMonster.push_back(MONSTER::ZB_RSH);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_GIRL);
        m_listSpawnMonster.push_back(MONSTER::ZB_BOY);
    }
    {
        m_listSpawnPos.push_back(Vec2(30.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 400.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
        m_listSpawnPos.push_back(Vec2(0.f, 300.f));
    } 
    {
        m_listSpawnBoxPos.push_back(Vec2(600.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(620.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(640.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(660.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(710.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(730.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(800.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1050.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1150.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1170.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1210.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1570.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1620.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1640.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1670.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1780.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1910.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(1980.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(2000.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(2020.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(2030.f, 300.f));
        m_listSpawnBoxPos.push_back(Vec2(2070.f, 300.f));

    }

    for (int i = 0; i < 16; ++i)
    {
        CSpawnBox* Box = new CSpawnBox;
        Box->SetPos(m_listSpawnBoxPos.front());
        Box->SetSpawnMonster(m_listSpawnMonster.front(),m_listSpawnPos.front());
        Instantiate(Box, m_listSpawnBoxPos.front(), LAYER::EDITUI);
        m_listSpawnMonster.pop_front();
        m_listSpawnBoxPos.pop_front();
        m_listSpawnPos.pop_front();
    }
}
