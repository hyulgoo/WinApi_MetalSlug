#include "pch.h"
#include "Game\04.Object\Character\Monster\CMonster.h"

#include "Game\02.Manager\02.TimeMgr\CTimeMgr.h"
#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\Character\Player\CPlayer.h"
#include "Game\05.Component\AI\CAI.h"
#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"
#include "Game\05.Component\Collider\CCollider.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"

#include <cfloat>
#include <cmath>

CMonster::CMonster()
    : m_iHP(0)
    , m_fSpeed(0.f)
    , m_fSlySpeed(0.f)
    , m_fAttackRange(0.f)
    , m_fDetectRange(0.f)
    , m_fDeathOffsetY(0.f)
    , m_bDir(false)
    , m_bFire(false)
    , m_iRand(0)
    , m_fRemoveScreenX(0.f)
    , m_iDamageBullet(0)
    , m_iDamageGrenade(0)
    , m_iDamageKnife(0)
{
    CreateAnimator();
    CreateCollider();
    CreateAI();

    // 생길 때 아이템 종류를 정해 둔다 (연구원 좀비가 남기는 상자). 값이 없거나 0이어도 0으로 나누지 않게
    m_iRand = rand() % max(1, CDataMgr::GetInst()->GetSection(L"monster", L"item").GetInt(L"roll"));

    m_fRemoveScreenX = CDataMgr::GetInst()->GetSection(L"monster", L"common").GetFloat(L"remove_screen_x");

    // 받는 피해
    const CDataSection& tDamage = CDataMgr::GetInst()->GetSection(L"monster", L"damage");
    m_iDamageBullet             = tDamage.GetInt(L"bullet");
    m_iDamageGrenade            = tDamage.GetInt(L"grenade");
    m_iDamageKnife              = tDamage.GetInt(L"knife");
}

CMonster::~CMonster()
{
}

void CMonster::LoadStats(const CDataSection& _tData)
{
    m_iHP           = _tData.GetInt(L"hp");
    m_fSpeed        = _tData.GetFloat(L"speed");
    m_fAttackRange  = _tData.GetFloat(L"attack_range");
    m_fDetectRange  = _tData.GetFloat(L"detect_range");
    m_fDeathOffsetY = _tData.GetFloat(L"death_offset_y");
    GetCollider()->SetOffsetPos(_tData.GetVec2(L"collider_offset"));
}

void CMonster::tick()
{
    CObj::tick();

    // 화면 왼쪽 밖으로 나가면 지운다
    if (GetScreenPos().x < m_fRemoveScreenX && !IsDead())
        SetDead();
}

void CMonster::BeginOverlap(CCollider* _pOther)
{
    // 예전에는 몬스터 여섯 종이 같은 코드를 따로 가졌다
    switch (_pOther->GetOwner()->GetLayer())
    {
    case LAYER::PLAYER_PROJECTILE:          // 권총·HMG
        m_iHP -= m_iDamageBullet;
        break;
    case LAYER::PLAYER_FIREPROJECTILE:      // 수류탄 (불에 타 죽는다)
        m_iHP -= m_iDamageGrenade;
        m_bFire = true;
        break;
    case LAYER::CQC:                        // 칼
        m_iHP -= m_iDamageKnife;
        m_bFire = false;
        break;
    default:
        break;
    }
}

CPlayer* CMonster::FindPlayer() const
{
    CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
    if (nullptr == pLevel)
        return nullptr;

    // 예전에는 상태마다 GetLayer(PLAYER)[0]을 범위 검사 없이 dynamic_cast 했다
    for (CObj* pObj : pLevel->GetLayer(LAYER::PLAYER))
    {
        if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj))
            return pPlayer;
    }
    return nullptr;
}

float CMonster::DistToPlayer() const
{
    CPlayer* pPlayer = FindPlayer();
    if (nullptr == pPlayer)
        return FLT_MAX;
    const Vec2 vDiff = pPlayer->GetPos() - GetPos();
    return vDiff.Length();
}

void CMonster::FacePlayer()
{
    CPlayer* pPlayer = FindPlayer();
    if (nullptr == pPlayer)
        return;

    m_bDir = pPlayer->GetPos().x - GetPos().x >= 0.f;

    // 쫓거나 기다리는 동안 플레이어가 지나가면 바로 돌아본다 (예전에는 상태가 바뀔 때만 방향을 정했다)
    ApplyFacing(GetAnimator(), m_bFacesRight[0]);
    ApplyFacing(GetAnimator2(), m_bFacesRight[1]);
}

void CMonster::MoveToPlayer(const float _fSpeed)
{
    CPlayer* pPlayer = FindPlayer();
    if (nullptr == pPlayer)
        return;

    Vec2        vDir = pPlayer->GetPos() - GetPos();
    const float fLen = vDir.Length();
    // 같은 자리면 방향이 없다 (예전에는 0으로 나눠 NaN이 위치에 퍼졌다)
    if (fLen <= 0.f)
        return;
    vDir = vDir / fLen;

    SetPos(GetPos() + vDir * (_fSpeed * DT));
}

void CMonster::NextAttackCycle()
{
    const float fDist = DistToPlayer();
    if (fDist < m_fAttackRange)
        GetAI()->ChangeState(ChooseAttack(fDist));   // 같은 공격이면 처음부터 다시
    else
        GetAI()->ChangeState(MON_STATE::TRACE);
}

void CMonster::Die()
{
    SpawnDeathEffect();
    if (!IsDead())
        SetDead();
}

void CMonster::PlayAnim(CAnimator* _pAnimator, const wchar_t* _szName, const bool _bRepeat, const bool _bFacesRight)
{
    if (nullptr == _pAnimator)
        return;
    _pAnimator->Play(_szName, _bRepeat);
    m_bFacesRight[_pAnimator == GetAnimator() ? 0 : 1] = _bFacesRight;
    ApplyFacing(_pAnimator, _bFacesRight);
}

void CMonster::ApplyFacing(CAnimator* _pAnimator, const bool _bFacesRight) const
{
    if (nullptr == _pAnimator || nullptr == _pAnimator->GetAnimation())
        return;

    // 그림이 보는 쪽과 플레이어 쪽이 다르면 뒤집는다 (돌아보지 않는 몬스터는 그대로)
    if (!m_bFixedFacing && m_bDir != _bFacesRight)
        _pAnimator->GetAnimation()->SetPlayLeft();
    else
        _pAnimator->GetAnimation()->SetPlayRight();
}
