#include "pch.h"
#include "CItem.h"
#include "CAnimator.h"
#include "CCamera.h"
#include "CResMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CPlayer.h"
#include "CTimeMgr.h"

CItem::CItem()
    : m_pAtlas(nullptr)
    , m_iBulletBomb(0)
    , m_bStart(true)
{
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();
    SetScale(Vec2(70.f, 70.f));
    m_pAtlas = CResMgr::GetInst()->LoadTexture(L"Weapone", L"texture\\Weapone.bmp");
    GetAnimator()->LoadAnimation(L"ITEM\\BOMB_ADD.anim");
    GetAnimator()->LoadAnimation(L"ITEM\\BUULLET_ADD.anim");
}

CItem::~CItem()
{
}

void CItem::render(const HDC _dc)
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * 4 * DT, GetPos().y));

    if (m_iBulletBomb > 5)
    {
        if (m_bStart)
        {
            GetAnimator()->Play(L"BUULLET_ADD.anim", true);
            m_bStart = false;
        }
    }
    else
    {
        if (m_bStart)
        {
            GetAnimator()->Play(L"BOMB_ADD.anim", true);
            m_bStart = false;
        }
    }
    CObj::render(_dc);
}

void CItem::BeginOverlap(CCollider* _pOther)
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

    if (m_iBulletBomb > 5)
    {
        dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0])->SetHMG();
        dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0])->AddBullet();
    }
    else
    {
        dynamic_cast<CPlayer*>(pCurLevel->GetLayer(LAYER::PLAYER)[0])->AddBomb();
    }

    if (!IsDead())
        SetDead();
}
