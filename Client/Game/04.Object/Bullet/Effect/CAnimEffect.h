#pragma once
#include "Game\04.Object\CObj.h"

// 애니메이션을 한 번 재생하고 사라지는 이펙트 (적 수류탄 폭발, 토사물이 땅·플레이어에 닿을 때).
// 예전에는 CSLDBombEffect·CZB_PRJ_Effect 두 클래스가 같은 코드를 따로 가졌다
class CAnimEffect :
    public CObj
{
private:
    wstring m_strAnim;      // 재생할 애니메이션 이름
    bool    m_bPlayRight;   // 그림을 뒤집지 않는다
    bool    m_bStart;

public:
    virtual void tick() override;

    CLONE_DEACTIVATE(CAnimEffect);

public:
    // _strAnimFile: animation 폴더 기준 .anim 경로
    CAnimEffect(const wstring& _strAnimFile, bool _bPlayRight);
    CAnimEffect(const CAnimEffect&) = delete;
    virtual ~CAnimEffect() override;
};
