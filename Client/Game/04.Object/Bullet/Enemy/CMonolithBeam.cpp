#include "pch.h"
#include "Game\04.Object\Bullet\Enemy\CMonolithBeam.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Bullet\Enemy\CMonolith.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Collider\PixelCollider\CPixelCollider.h"

namespace
{
    const CPlayer* FindPlayer()
    {
        CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
        if (nullptr == pLevel)
            return nullptr;
        for (CObj* pObj : pLevel->GetLayer(LAYER::PLAYER))
        {
            if (const CPlayer* pPlayer = dynamic_cast<const CPlayer*>(pObj))
                return pPlayer;
        }
        return nullptr;
    }
}

CMonolithBeam::CMonolithBeam()
{
    const CDataSection& tData = CDataMgr::GetInst()->GetSection(L"boss", L"beam");
    m_tData.fTrackTime        = tData.GetFloat(L"track_time");
    m_tData.fTrackSpeed       = tData.GetFloat(L"track_speed");
    m_tData.fLockTime         = tData.GetFloat(L"lock_time");

    // 빔은 Animator, 땅의 표식은 Animator2 (표식을 먼저, 빔을 위에 그린다)
    CreateAnimator();
    CreateAnimator2();
    GetAnimator()->LoadAnimation(L"Boss\\MONOLITH_BEAM.anim");
    GetAnimator2()->LoadAnimation(L"Boss\\MONOLITH_BEACON.anim");
}

CMonolithBeam::~CMonolithBeam()
{
}

void CMonolithBeam::tick()
{
    if (!m_bStart)
    {
        m_bStart = true;
        GetAnimator()->Play(L"MONOLITH_BEAM.anim", true);
        GetAnimator2()->Play(L"MONOLITH_BEACON.anim", true);
    }

    m_fTime += DT;

    // 따라오는 동안은 플레이어 쪽으로 움직인다 (최고 속도가 플레이어 걷기보다 느려서 달아날 수 있다)
    if (m_fTime <= m_tData.fTrackTime)
    {
        if (const CPlayer* pPlayer = FindPlayer())
        {
            const float fDiff = pPlayer->GetPos().x - GetPos().x;
            const float fMax  = m_tData.fTrackSpeed * DT;
            SetPos(Vec2(GetPos().x + max(-fMax, min(fDiff, fMax)), GetPos().y));
        }
    }

    // 빔 끝은 그 x의 땅 (화면 위 끝부터 찾는다)
    float fGround = 0.f;
    if (CPixelCollider::FindGroundBelow(GetPos().x, CCamera::GetInst()->GetRealPos(Vec2(0.f, 0.f)).y, fGround))
        SetPos(Vec2(GetPos().x, fGround));

    // 멈추고 lock_time 뒤에 빔이 사라지고 그 자리로 비석이 떨어진다 (비석은 다음 프레임에 화면 위 밖에서 떨어지기 시작한다.
    // 예전에는 비석을 떨어뜨린 뒤에도 빔이 0.3초 남아, 떨어지는 비석이 빔을 지나갔다)
    if (!IsDead() && m_fTime >= m_tData.fTrackTime + m_tData.fLockTime)
    {
        Instantiate(new CMonolith, GetPos(), LAYER::OBSTACLE);
        SetDead();
    }

    CObj::tick();
}
