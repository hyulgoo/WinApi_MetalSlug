#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CSLDCQC.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

void CSLDCQC::BeginOverlap(CCollider* _pOther)
{
}

CSLDCQC::CSLDCQC()
    : m_bStart(false)
    , m_fOffset()
    , m_bDir(false)
    , m_fTime(0.f)
{
    SetScale(Vec2(40.f, 40.f));
    CreateCollider();
}

CSLDCQC::~CSLDCQC()
{
}

void CSLDCQC::tick()
{
    if (m_bStart == false)
    {
        if (m_bDir)
            m_fOffset = 40.f;
        else
            m_fOffset = -40.f;
        SetPos(Vec2(GetPos().x + m_fOffset, GetPos().y));
        m_bStart = true;
    }

    m_fTime += DT;
    if (0.001f < m_fTime && !IsDead())
        SetDead();

    CObj::tick();
}

void CSLDCQC::render(const HDC _dc)
{
    CObj::render(_dc);
}
