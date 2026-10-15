#include "pch.h"
#include "Game\04.Object\Bullet\Player\CCQCArea.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include "Game\04.Object\Character\Player\CPlayer.h"

CCQCArea::CCQCArea()
    : m_bCQC(false)
{
    // 자리는 플레이어가 매 tick 몸 앞에 둔다 (예전에 여기서 옮기던 40은 바로 덮어써져 쓰이지 않았다)
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"weapon", L"knife_area");
    CreateCollider();
    GetCollider()->SetOffsetPos(tData.GetVec2(L"offset"));
    SetScale(tData.GetVec2(L"size"));
}

CCQCArea::~CCQCArea()
{
}

void CCQCArea::tick()
{
    CObj::tick();
}

void CCQCArea::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CCQCArea::OnOverlap(CCollider* /*_pOther*/)
{
    m_bCQC = true;
}

void CCQCArea::EndOverlap(CCollider* /*_pOther*/)
{
    m_bCQC = false;
}
