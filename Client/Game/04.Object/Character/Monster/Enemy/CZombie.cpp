#include "pch.h"
#include "Game\04.Object\Character\Monster\Enemy\CZombie.h"

#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"

#include "Game\04.Object\Bullet\Enemy\CZB_Bullet.h"
#include "Game\04.Object\Character\Monster\Enemy\CDeadEffect.h"
#include "Game\04.Object\EventBox\CItem.h"

namespace
{
    // 발사 프레임: 예전 타이머(2.64·1.82·1.82·1.08·1.82초)와 같거나 가장 가까운 공격 애니메이션 프레임 시작 (Stage 3)
    const tZombieDesc g_arrDesc[] =
    {
        // 소년: 2.64초 = 22번 (0.12초 x 22)
        { L"CZombie_boy", L"ZB_BOY_IDLE.anim", L"ZB_BOY_RUN.anim", L"ZB_BOY_ATTACK_EFFECT.anim", true, L"ZB_BOY_ATTACK.anim", 22
        , Vec2(70.f, 130.f), Vec2(50.f, -90.f), false },
        // 소녀: 1.82초 = 14번 (0.13초 x 14)
        { L"CZombie_girl", L"ZB_GIRL_IDLE.anim", L"ZB_GIRL_RUN.anim", L"ZB_GIRL_ATTACK_EFFECT.anim", true, L"ZB_GIRL_ATTACK.anim", 14
        , Vec2(60.f, 130.f), Vec2(70.f, -70.f), false },
        // 남자: 1.82초에 가장 가까운 18번 (1.86초)
        { L"CZombie_man", L"ZB_MAN_IDLE.anim", L"ZB_MAN_RUN.anim", L"ZB_MAN_ATTACK_EFFECT.anim", true, L"ZB_MAN_ATTACK.anim", 18
        , Vec2(70.f, 130.f), Vec2(30.f, -70.f), false },
        // 노인: 1.08초 = 9번 (0.12초 x 9)
        { L"CZombie_oldman", L"ZB_OLD_IDLE.anim", L"ZB_OLD_RUN.anim", L"ZB_OLD_ATTACK_EFFECT.anim", false, L"ZB_OLD_ATTACK.anim", 9
        , Vec2(90.f, 130.f), Vec2(30.f, -70.f), false },
        // 연구원: 1.82초에 가장 가까운 15번 (1.80초). 죽으면 아이템
        { L"CZombie_RSH", L"ZB_RCH_IDLE.anim", L"ZB_RCH_RUN.anim", L"ZB_RCH_ATTACK_EFFECT.anim", false, L"ZB_RCH_ATTACK.anim", 15
        , Vec2(70.f, 130.f), Vec2(30.f, -70.f), true },
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
{
    SetScale(m_tDesc.vScale);
    GetCollider()->SetOffsetPos(Vec2(0.f, -15.f));

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

    SetStats(10, 80.f, 300.f, 700.f);

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
    return m_tDesc.iFireFrame;
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

    const Vec2 vOffset = Vec2(bRight ? m_tDesc.vBulletOffset.x : -m_tDesc.vBulletOffset.x, m_tDesc.vBulletOffset.y);
    Instantiate(pBullet, GetPos() + vOffset, LAYER::MONSTER_PROJECTILE);
}

void CZombie::SpawnDeathEffect()
{
    // 발밑(아래로 70)에 사망 이펙트를 둔다. 예전 연구원 좀비만 불에 타 죽을 때 70을 빠뜨렸다
    const Vec2 vPos = Vec2(GetPos().x, GetPos().y + 70.f);

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
