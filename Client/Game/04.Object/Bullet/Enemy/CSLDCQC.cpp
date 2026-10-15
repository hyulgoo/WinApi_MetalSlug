#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CSLDCQC.h"
#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

void CSLDCQC::BeginOverlap(CCollider* /*_pOther*/)
{
}

CSLDCQC::CSLDCQC()
    : m_bStart(false)
    , m_fOffset(0.f)
    , m_bDir(false)
    , m_fTime(0.f)
    , m_fLifeTime(0.f)
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"soldier_knife");
    m_fOffset                 = tData.GetFloat(L"offset_x");
    m_fLifeTime               = tData.GetFloat(L"life_time");
    SetScale(tData.GetVec2(L"size"));
    CreateCollider();
}

CSLDCQC::~CSLDCQC()
{
}

void CSLDCQC::tick()
{
    if (m_bStart == false)
    {
        SetPos(Vec2(GetPos().x + (m_bDir ? m_fOffset : -m_fOffset), GetPos().y));
        m_bStart = true;
    }

    m_fTime += DT;
    if (m_fLifeTime < m_fTime && !IsDead())
        SetDead();

    CObj::tick();
}

void CSLDCQC::render(const HDC _dc)
{
    CObj::render(_dc);
}
