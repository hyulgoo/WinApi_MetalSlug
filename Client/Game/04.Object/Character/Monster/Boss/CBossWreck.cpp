#include "pch.h"
#include "Game\04.Object\Character\Monster\Boss\CBossWreck.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\02.Manager\08.Camera\CCamera.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\05.Component\Animator\CAnimator.h"

#include <cmath>

CBossWreck::CBossWreck()
{
    const CDataSection& tData  = CDataMgr::GetInst()->GetSection(L"boss", L"wreck");
    m_tData.fExplosionTime     = tData.GetFloat(L"explosion_time");
    m_tData.fExplosionInterval = tData.GetFloat(L"explosion_interval");
    m_tData.vExplosionArea     = tData.GetVec2(L"explosion_area");
    m_tData.vExplosionOffset   = tData.GetVec2(L"explosion_offset");
    m_tData.fShakeRange        = tData.GetFloat(L"shake_range");
    m_tData.fShakeSpeed        = tData.GetFloat(L"shake_speed");
    m_tData.fVictoryDelay      = tData.GetFloat(L"victory_delay");

    CreateAnimator();
    GetAnimator()->LoadAnimation(L"Boss\\BOSS_WRECK.anim");
}

CBossWreck::~CBossWreck()
{
}

void CBossWreck::tick()
{
    if (!m_bStart)
    {
        m_bStart = true;
        GetAnimator()->Play(L"BOSS_WRECK.anim", true);
        CCamera::GetInst()->CameraShake(m_tData.fShakeRange, m_tData.fShakeSpeed, m_tData.fExplosionTime);
    }

    m_fTime += DT;
    while (m_tData.fExplosionInterval > 0.f && m_fNextExplosion <= m_fTime && m_fNextExplosion < m_tData.fExplosionTime)
    {
        SpawnExplosion();
        m_fNextExplosion += m_tData.fExplosionInterval;
    }

    // 폭발이 끝나고 잠시 뒤 플레이어가 승리 포즈를 한다
    if (!m_bVictory && m_fTime >= m_tData.fExplosionTime + m_tData.fVictoryDelay)
    {
        m_bVictory = true;
        if (CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel())
        {
            for (CObj* pObj : pLevel->GetLayer(LAYER::PLAYER))
            {
                if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj))
                    pPlayer->SetVictory();
            }
        }
    }

    CObj::tick();
}

void CBossWreck::SpawnExplosion()
{
    // 폭발 자리: 상자 안을 고르게 덮도록 무리수 간격으로 돈다 (난수를 쓰지 않아 늘 같은 자리)
    const float fU   = fmodf(static_cast<float>(m_iExplosion) * 0.618034f, 1.f);
    const float fV   = fmodf(static_cast<float>(m_iExplosion) * 0.414214f + 0.5f, 1.f);
    const Vec2  vPos = GetPos() + m_tData.vExplosionOffset
                     + Vec2((fU - 0.5f) * m_tData.vExplosionArea.x, (fV - 0.5f) * m_tData.vExplosionArea.y);

    // 푸른 폭발과 주황 폭발을 번갈아 (그림의 아래 가운데가 그 자리)
    const wchar_t* szAnim = 0 == m_iExplosion % 2 ? L"Boss\\BOSS_EXPLOSION_A.anim" : L"Boss\\BOSS_EXPLOSION_B.anim";
    Instantiate(new CAnimEffect(szAnim, true), vPos, LAYER::EFFECT);
    ++m_iExplosion;
}
