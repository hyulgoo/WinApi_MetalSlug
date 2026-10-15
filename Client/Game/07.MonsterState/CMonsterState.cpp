#include "pch.h"
#include "Game\07.MonsterState\CMonsterState.h"

#include "Game\04.Object\Character\Monster\CMonster.h"
#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"

namespace
{
    // 플레이어를 기다린다. 감지 범위에 들어오면 쫓는다
    class CIdleState final : public CMonsterState
    {
    public:
        void Enter(CMonster& _mon) const override
        {
            _mon.FacePlayer();
            _mon.PlayStateAnim(MON_STATE::IDLE);
        }

        void Update(CMonster& _mon) const override
        {
            _mon.FacePlayer();
            if (_mon.DistToPlayer() < _mon.GetDetectRange())
                _mon.GetAI()->ChangeState(MON_STATE::TRACE);
        }
    };

    // 쫓아간다. 공격 범위에 들어오면 공격(무엇으로 할지는 몬스터가 고른다), 감지 범위를 벗어나면 기다린다
    class CTraceState final : public CMonsterState
    {
    public:
        void Enter(CMonster& _mon) const override
        {
            _mon.FacePlayer();
            _mon.PlayStateAnim(MON_STATE::TRACE);
        }

        void Update(CMonster& _mon) const override
        {
            // 판정은 이번 프레임에 움직이기 전 거리로 한다 (예전 순서와 같다)
            const float fDist = _mon.DistToPlayer();
            _mon.MoveToPlayer(_mon.GetSpeed());
            _mon.FacePlayer();

            if (fDist < _mon.GetAttackRange())
                _mon.GetAI()->ChangeState(_mon.ChooseAttack(fDist));
            else if (fDist > _mon.GetDetectRange())
                _mon.GetAI()->ChangeState(MON_STATE::IDLE);
        }
    };

    // 천천히(절반 속도) 다가간다 (병사). 칼 거리에 들어오면 칼, 공격 범위를 벗어나면 다시 쫓는다
    class CTraceSlyState final : public CMonsterState
    {
    public:
        void Enter(CMonster& _mon) const override
        {
            _mon.FacePlayer();
            _mon.PlayStateAnim(MON_STATE::TRACE_SLY);
        }

        void Update(CMonster& _mon) const override
        {
            const float fDist = _mon.DistToPlayer();
            _mon.MoveToPlayer(_mon.GetSpeed() / 2.f);
            _mon.FacePlayer();

            if (fDist < _mon.GetAttackRange())
            {
                if (_mon.ChooseAttack(fDist) == MON_STATE::ATTACK_CQC)
                    _mon.GetAI()->ChangeState(MON_STATE::ATTACK_CQC);
            }
            else
            {
                _mon.GetAI()->ChangeState(MON_STATE::TRACE);
            }
        }
    };

    // 공격 한 번 = 공격 애니메이션 한 번. 정해진 프레임에서 쏘고, 애니메이션이 끝나면 다음 공격을 고른다.
    // 예전에는 공격 반복을 "공격 지연 타이머가 끝나면 Trace로 갔다가 다시 Attack"으로 구현해 사이클마다
    // 달리기 애니메이션이 한 프레임 끼었고, 발사 시점은 몬스터 쪽 타이머가 따로 정했다
    class CAttackStateBase : public CMonsterState
    {
    protected:
        virtual MON_STATE Kind() const = 0;

    public:
        void Enter(CMonster& _mon) const override
        {
            // 공격을 시작할 때 플레이어 쪽을 보고, 공격하는 동안은 방향을 바꾸지 않는다
            _mon.FacePlayer();
            _mon.PlayStateAnim(Kind());
        }

        void Update(CMonster& _mon) const override
        {
            const CAnimation* pAnim = _mon.GetAttackAnim();
            if (nullptr != pAnim && pAnim->IsFrameEntered(_mon.GetAttackFrame(Kind())))
                _mon.OnAttackFrame(Kind());

            if (nullptr == pAnim || pAnim->IsFinish())
                _mon.NextAttackCycle();
        }
    };

    class CAttackState final : public CAttackStateBase
    {
    protected:
        MON_STATE Kind() const override { return MON_STATE::ATTACK; }
    };

    class CAttackCQCState final : public CAttackStateBase
    {
    protected:
        MON_STATE Kind() const override { return MON_STATE::ATTACK_CQC; }
    };

    // 죽는다: 사망 이펙트(와 아이템)를 남기고 사라진다
    class CDeadState final : public CMonsterState
    {
    public:
        void Enter(CMonster& _mon) const override { _mon.Die(); }
        void Update(CMonster& /*_mon*/) const override {}
    };

    const CIdleState      g_Idle;
    const CTraceState     g_Trace;
    const CTraceSlyState  g_TraceSly;
    const CAttackState    g_Attack;
    const CAttackCQCState g_AttackCQC;
    const CDeadState      g_Dead;
}

const CMonsterState* CMonsterState::Get(const MON_STATE _eState)
{
    switch (_eState)
    {
    case MON_STATE::IDLE:       return &g_Idle;
    case MON_STATE::TRACE:      return &g_Trace;
    case MON_STATE::TRACE_SLY:  return &g_TraceSly;
    case MON_STATE::ATTACK:     return &g_Attack;
    case MON_STATE::ATTACK_CQC: return &g_AttackCQC;
    case MON_STATE::DEAD:       return &g_Dead;
    default:                    return nullptr;
    }
}
