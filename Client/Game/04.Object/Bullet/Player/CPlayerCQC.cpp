#include "pch.h"
#include "Game\04.Object\Bullet\Player\CPlayerCQC.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

CPlayerCQC::CPlayerCQC()
    : m_fTime(0.f)
    , m_fLifeTime(0.f)
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"knife");
    m_fLifeTime               = tData.GetFloat(L"life_time");
    CreateCollider();
    SetScale(tData.GetVec2(L"size"));
    GetCollider()->SetOffsetPos(tData.GetVec2(L"offset"));
}

CPlayerCQC::~CPlayerCQC()
{
}

void CPlayerCQC::tick()
{
    m_fTime += DT;
    if (m_fLifeTime < m_fTime && !IsDead())
        SetDead();
    CObj::tick();
}


void CPlayerCQC::BeginOverlap(CCollider* /*_pOther*/)
{
}
