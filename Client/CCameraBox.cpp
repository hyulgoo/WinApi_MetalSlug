#include "pch.h"
#include "CCameraBox.h"

#include "CCamera.h"
#include "CTimeMgr.h"

CCameraBox::CCameraBox()
    : m_bUpDown(false)
    , m_fDuration(0.f)
    , m_fMoveSpeed(0.f)
{
    CreateCollider();
    SetScale(Vec2(2.f, 10000.f));
}

CCameraBox::~CCameraBox()
{
}

void CCameraBox::tick()
{
    if (CCamera::GetInst()->IsCameraStop() == false)
        SetPos(Vec2(GetPos().x - CCamera::GetInst()->GetCameraSpeed() * DT, GetPos().y));

    CObj::tick();
}

void CCameraBox::render(const HDC _dc)
{
    CObj::render(_dc);
}

void CCameraBox::BeginOverlap(CCollider* _pOther)
{
    CCamera::GetInst()->MoveCamera(m_bUpDown, m_fDuration, m_fMoveSpeed);
    if (!IsDead())
        SetDead();
}
