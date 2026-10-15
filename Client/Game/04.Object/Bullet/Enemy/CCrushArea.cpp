#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CCrushArea.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"

CCrushArea::CCrushArea(const Vec2& _vSize, const float _fLifeTime)
    : m_fLifeTime(_fLifeTime)
{
    SetScale(_vSize);
    CreateCollider();
}

CCrushArea::~CCrushArea()
{
}

void CCrushArea::tick()
{
    m_fTime += DT;
    if (m_fLifeTime < m_fTime && !IsDead())
        SetDead();

    CObj::tick();
}
