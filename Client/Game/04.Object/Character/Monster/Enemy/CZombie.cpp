#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\04.Object\Bullet\Enemy\CZB_Bullet.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"
#include "Game\04.Object\EventBox\CItem.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

namespace
{
    // 공격 프레임·크기·토사물 자리는 data\monster.ini 의 구역에 있다
    const tZombieDesc g_arrDesc[] =
    {
        { L"CZombie_boy", L"zombie_boy", L"ZB_BOY_IDLE.anim", L"ZB_BOY_RUN.anim", L"ZB_BOY_ATTACK_EFFECT.anim", true, L"ZB_BOY_ATTACK.anim", false },
        { L"CZombie_girl", L"zombie_girl", L"ZB_GIRL_IDLE.anim", L"ZB_GIRL_RUN.anim", L"ZB_GIRL_ATTACK_EFFECT.anim", true, L"ZB_GIRL_ATTACK.anim", false },
        { L"CZombie_man", L"zombie_man", L"ZB_MAN_IDLE.anim", L"ZB_MAN_RUN.anim", L"ZB_MAN_ATTACK_EFFECT.anim", true, L"ZB_MAN_ATTACK.anim", false },
        { L"CZombie_oldman", L"zombie_old", L"ZB_OLD_IDLE.anim", L"ZB_OLD_RUN.anim", L"ZB_OLD_ATTACK_EFFECT.anim", false, L"ZB_OLD_ATTACK.anim", false },
        // 연구원: 죽으면 아이템
        { L"CZombie_RSH", L"zombie_researcher", L"ZB_RCH_IDLE.anim", L"ZB_RCH_RUN.anim", L"ZB_RCH_ATTACK_EFFECT.anim", false, L"ZB_RCH_ATTACK.anim", true },
    };

    constexpr const wchar_t* NOTHING = L"ZB_NOTHING.anim";   // 비어 있는 애니메이션 (Animator2를 비울 때)
}

const tZombieDesc& GetZombieDesc(const MONSTER _eType)
{
    switch (_eType)
    {
    case MONSTER::ZB_GIRL: return g_arrDesc[1];
    case MONSTER::ZB_MAN:  return g_arrDesc[2];
    case MONSTER::ZB_OLD:  return g_arrDesc[3];
    case MONSTER::ZB_RSH:  return g_arrDesc[4];
    case MONSTER::ZB_BOY:
    default:               return g_arrDesc[0];
    }
}

CZombie::CZombie(const MONSTER _eType)
    : m_tDesc(GetZombieDesc(_eType))
    , m_iFireFrame(0)
{
    const CDataSection& tType = CDataMgr::GetInst()->GetSection(L"monster", m_tDesc.szData);
    m_iFireFrame              = tType.GetInt(L"fire_frame");
    m_vBulletOffset           = tType.GetVec2(L"bullet_offset");
    SetScale(tType.GetVec2(L"size"));

    // 체력·속도·범위·충돌체 자리는 다섯 종이 같다
    LoadStats(CDataMgr::GetInst()->GetSection(L"monster", L"zombie"));

    CreatePixelCollider();
    CreateRigidbody();
    CreateAnimator2();

    // 몸·이펙트는 Animator, 공격하는 몸은 Animator2 (Animator2를 먼저, Animator를 위에 그린다)
    // 예전에는 연구원만 기다리기·달리기를 Animator2에 두어 두 애니메이터의 역할이 반대였다
    const wstring strDir = L"Enemy//";
    GetAnimator()->LoadAnimation(strDir + m_tDesc.szIdle);
    GetAnimator()->LoadAnimation(strDir + m_tDesc.szRun);
    GetAnimator()->LoadAnimation(strDir + m_tDesc.szAttackEffect);
    GetAnimator2()->LoadAnimation(strDir + NOTHING);
    GetAnimator2()->LoadAnimation(strDir + m_tDesc.szAttackBody);

    GetAI()->AddState(MON_STATE::IDLE);
    GetAI()->AddState(MON_STATE::TRACE);
    GetAI()->AddState(MON_STATE::ATTACK);
    GetAI()->AddState(MON_STATE::DEAD);
    GetAI()->ChangeState(MON_STATE::IDLE);
}

CZombie::~CZombie()
{
}

void CZombie::PlayStateAnim(const MON_STATE _eState)
{
    // 좀비 그림은 모두 왼쪽을 보고 그려졌다
    switch (_eState)
    {
    case MON_STATE::IDLE:
        PlayAnim(GetAnimator(), m_tDesc.szIdle, true, false);
        PlayAnim(GetAnimator2(), NOTHING, false, false);
        break;
    case MON_STATE::TRACE:
        PlayAnim(GetAnimator(), m_tDesc.szRun, true, false);
        PlayAnim(GetAnimator2(), NOTHING, false, false);
        break;
    case MON_STATE::ATTACK:
        PlayAnim(GetAnimator(), m_tDesc.szAttackEffect, m_tDesc.bAttackEffectRepeat, false);
        PlayAnim(GetAnimator2(), m_tDesc.szAttackBody, false, false);
        break;
    default:
        break;
    }
}

const CAnimation* CZombie::GetAttackAnim() const
{
    return GetAnimator2()->GetAnimation();
}

int CZombie::GetAttackFrame(const MON_STATE /*_eState*/) const
{
    return m_iFireFrame;
}

void CZombie::OnAttackFrame(const MON_STATE /*_eState*/)
{
    // 토사물: 그림 방향 플래그는 몬스터와 반대로 쓴다(토사물 그림이 오른쪽을 보고 그려졌다)
    const bool  bRight  = GetDir();
    CZB_Bullet* pBullet = new CZB_Bullet;
    pBullet->SetDir(!bRight);
    if (bRight)
        pBullet->AddVelocityRight();
    else
        pBullet->AddVelocityLeft();

    const Vec2 vOffset = Vec2(bRight ? m_vBulletOffset.x : -m_vBulletOffset.x, m_vBulletOffset.y);
    Instantiate(pBullet, GetPos() + vOffset, LAYER::MONSTER_PROJECTILE);
}

void CZombie::SpawnDeathEffect()
{
    // 발밑에 사망 이펙트를 둔다. 예전 연구원 좀비만 불에 타 죽을 때 발밑으로 내리는 것을 빠뜨렸다
    const Vec2 vPos = Vec2(GetPos().x, GetPos().y + GetDeathOffsetY());

    CDeadEffect* pEffect = new CDeadEffect;
    pEffect->SetType(2);
    if (IsFire())
        pEffect->SetFire();
    pEffect->SetDir(GetDir());
    Instantiate(pEffect, vPos, LAYER::DEFAULT);

    if (m_tDesc.bDropItem)
    {
        CItem* pItem = new CItem;
        pItem->SetBB(GetRand());
        Instantiate(pItem, vPos, LAYER::ITEM);
    }
}
