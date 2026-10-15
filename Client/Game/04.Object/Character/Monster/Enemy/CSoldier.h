#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

// 반란군 병사: 멀면 수류탄, 가까우면 천천히 다가가 칼
class CSoldier :
    public CMonster
{
private:
    // 수치 (data\monster.ini [soldier]). 만들 때 읽는다
    int   m_iGrenadeFrame;    // SOLDIER_BOMB.anim: 수류탄을 놓는 프레임
    int   m_iKnifeFrame;      // SOLDIER_CQC.anim: 칼을 휘두르는 프레임
    float m_fCQCRatio;        // 공격 범위에 대한 비율: 이보다 가까우면 칼
    float m_fSlyRatio;        // 그다음 이 비율까지는 천천히 다가간다
    Vec2  m_vGrenadeOffset;   // 수류탄이 나오는 자리 (오른쪽을 볼 때)

public:
    virtual void              PlayStateAnim(MON_STATE _eState) override;
    virtual const CAnimation* GetAttackAnim() const override;
    virtual int               GetAttackFrame(MON_STATE _eState) const override;
    virtual void              OnAttackFrame(MON_STATE _eState) override;
    // 공격 범위 안에서 거리로 고른다: 칼 거리 안이면 칼, 그다음까지는 천천히 다가가기, 그 밖은 수류탄
    virtual MON_STATE         ChooseAttack(float _fDist) const override;

protected:
    virtual void SpawnDeathEffect() override;

public:
    CLONE_DEACTIVATE(CSoldier);

public:
    CSoldier();
    CSoldier(const CSoldier&) = delete;  // 복제하지 않는다
    virtual ~CSoldier() override;
};
