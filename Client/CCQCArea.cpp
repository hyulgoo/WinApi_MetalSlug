#include "pch.h"
#include "CCQCArea.h"
#include "CCollider.h"

#include "CPlayer.h"

CCQCArea::CCQCArea()
    : m_bCQC(false)
    , m_fTime(0.f)
{
    CreateCollider();
    SetPos(Vec2(GetPos().x + 40, GetPos().y));
    GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));
    SetScale(Vec2(120.f, 120.f));
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

void CCQCArea::OnOverlap(CCollider* _pOther)
{
    m_bCQC = true;
}

void CCQCArea::EndOverlap(CCollider* _pOther)
{
    m_bCQC = false;
}
