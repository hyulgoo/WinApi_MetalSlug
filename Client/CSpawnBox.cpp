#include "pch.h"
#include "CSpawnBox.h"
#include "CCamera.h"
#include "CTimeMgr.h"
#include "CSoldier.h"
#include "CZombie_boy.h"
#include "CZombie_girl.h"
#include "CZombie_man.h"
#include "CZombie_oldman.h"
#include "CZombie_RSH.h"

CSpawnBox::CSpawnBox()
{
    CreateCollider();
    SetScale(Vec2(2.f, 2000.f));

    AddMonster(L"Soldier", new CSoldier);
    AddMonster(L"Zombie_boy", new CZombie_boy);
    AddMonster(L"Zombie_man", new CZombie_man);
    AddMonster(L"Zombie_oldman", new CZombie_oldman);
    AddMonster(L"Zombie_RSH", new CZombie_RSH);
    AddMonster(L"Zombie_girl", new CZombie_girl);
}

CSpawnBox::CSpawnBox(const CSpawnBox& _other)
{
    CreateCollider();
    SetScale(Vec2(2.f, 2000.f));

    AddMonster(L"Soldier", new CSoldier);
    AddMonster(L"Zombie_boy", new CZombie_boy);
    AddMonster(L"Zombie_man", new CZombie_man);
    AddMonster(L"Zombie_oldman", new CZombie_oldman);
    AddMonster(L"Zombie_RSH", new CZombie_RSH);
    AddMonster(L"Zombie_girl", new CZombie_girl);
}

CSpawnBox::~CSpawnBox()
{
    for (const auto obj : m_mapObj)
    {
        DEL(obj.second)
    }
    m_mapObj.clear();
}

void CSpawnBox::tick()
{
    CObj::tick();

    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * DT, GetPos().y));
}

void CSpawnBox::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CSpawnBox::Create(const wstring& _strKey, const Vec2& vPos)
{
    CObj* Monster = FindMonster(_strKey)->Clone();
    Instantiate(Monster, vPos, LAYER::MONSTER);
}

void CSpawnBox::SpawnMonster(const MONSTER _tMonster, const Vec2& m_vMonPos)
{
    Vec2 vPos = GetPos() + Vec2(900.f, 0.f);
    vPos      += m_vMonPos;
    switch (_tMonster)
    {
    case MONSTER::SOLDIER:
        Create(L"Soldier", vPos);
        break;
    case MONSTER::ZB_GIRL:
        Create(L"Zombie_girl", vPos);
        break;
    case MONSTER::ZB_BOY:
        Create(L"Zombie_boy", vPos);
        break;
    case MONSTER::ZB_MAN:
        Create(L"Zombie_man", vPos);
        break;
    case MONSTER::ZB_OLD:
        Create(L"Zombie_oldman", vPos);
        break;
    case MONSTER::ZB_RSH:
        Create(L"Zombie_RSH", vPos);
        break;
    default: ;
    }
}

void CSpawnBox::BeginOverlap(CCollider* _pOther)
{
    SpawnMonster(m_strMon, m_vMonPos);
    if (!IsDead())
        SetDead();
}
