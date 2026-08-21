#include "pch.h"
#include "Game\04.Object\Bullet\Player\CPlayerCQC.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"

CPlayerCQC::CPlayerCQC()
    : m_fSpeed(200.f)
    , m_fDegree(80.f)
    , m_fTime(0.f)
    , m_pTexture(nullptr)
    , m_bStart(false)
{
    CreateCollider();
    SetScale(Vec2(120.f, 120.f));
    GetCollider()->SetOffsetPos(Vec2(0.f, 10.f));
    SetSpeed(1300.f);
}

CPlayerCQC::~CPlayerCQC()
{
}

void CPlayerCQC::tick()
{
    m_fTime += DT;
    if (0.05f < m_fTime && !IsDead())
        SetDead();
    CObj::tick();
}


void CPlayerCQC::BeginOverlap(CCollider* _pOther)
{
}
