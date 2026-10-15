#include "pch.h"
#include "Game\04.Object\EventBox\CItem.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

CItem::CItem()
    : m_iBulletBomb(0)
    , m_bStart(true)
{
    CreateAnimator();
    CreateCollider();
    CreatePixelCollider();
    CreateRigidbody();
    SetScale(Vec2(70.f, 70.f));
    GetAnimator()->LoadAnimation(L"ITEM\\BOMB_ADD.anim");
    GetAnimator()->LoadAnimation(L"ITEM\\BULLET_ADD.anim");
}

CItem::~CItem()
{
}

void CItem::tick()
{
    // 처음 tick에 종류(총알·폭탄)에 맞는 애니메이션을 고른다 (예전에는 render에서 재생해 그리기가 상태를 바꿨다)
    if (m_bStart)
    {
        GetAnimator()->Play(m_iBulletBomb > 5 ? L"BULLET_ADD.anim" : L"BOMB_ADD.anim", true);
        m_bStart = false;
    }
    CObj::tick();
}

void CItem::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CItem::BeginOverlap(CCollider* /*_pOther*/)
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
