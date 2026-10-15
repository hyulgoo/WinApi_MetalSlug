#pragma once
#include "Game\04.Object\Character\Monster\CMonster.h"

// 좀비 종류마다 다른 것. 예전 다섯 클래스(CZombie_boy·girl·man·oldman·RSH)는 이름을 맞추면 90% 넘게 같았고
// 실제 차이는 아래 값과 수치(공격 프레임, 크기, 토사물 자리)뿐이었다(복사할 때마다 다른 버그가 들어갔다: MS-R12, R13)
struct tZombieDesc
{
    const wchar_t* szTestName;          // 로그·테스트에서 부르는 이름 (예전 클래스 이름)
    const wchar_t* szData;              // 수치가 있는 data\monster.ini 구역 (fire_frame, size, bullet_offset)
    const wchar_t* szIdle;              // 기다릴 때 (Animator)
    const wchar_t* szRun;               // 쫓을 때 (Animator)
    const wchar_t* szAttackEffect;      // 토하는 이펙트 (Animator, 몸 위에 그린다)
    bool           bAttackEffectRepeat;
    const wchar_t* szAttackBody;        // 토하는 몸 (Animator2). 공격 시점과 길이를 정한다
    bool           bDropItem;           // 죽으면 아이템 상자를 남긴다 (연구원)
};

// 좀비 다섯 종 (MONSTER::ZB_* 순서와 상관없이 종류로 찾는다)
const tZombieDesc& GetZombieDesc(MONSTER _eType);

class CZombie :
    public CMonster
{
private:
    const tZombieDesc& m_tDesc;
    int                m_iFireFrame;      // szAttackBody가 이 프레임에 들어설 때 토사물이 나간다
    Vec2               m_vBulletOffset;   // 토사물이 나오는 자리 (오른쪽을 볼 때. 왼쪽이면 x를 뒤집는다)

public:
    const tZombieDesc& GetDesc() const { return m_tDesc; }

    virtual void              PlayStateAnim(MON_STATE _eState) override;
    virtual const CAnimation* GetAttackAnim() const override;
    virtual int               GetAttackFrame(MON_STATE _eState) const override;
    virtual void              OnAttackFrame(MON_STATE _eState) override;

protected:
    virtual void SpawnDeathEffect() override;

public:
    CLONE_DEACTIVATE(CZombie);

public:
    explicit CZombie(MONSTER _eType);
    CZombie(const CZombie&) = delete;
    virtual ~CZombie() override;
};
