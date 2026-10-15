#pragma once
#include "Game\04.Object\CObj.h"

// 애니메이션 에디터의 미리보기. 편집 중인 애니메이션은 GetAnimator(), 함께 볼 참고 애니메이션은 GetAnimator2()에 둔다.
// 아틀라스는 애니메이션마다 따로 가진다 (예전에는 이 클래스에 Soldier.bmp를 코드로 고정해 두고, 저장할 때 그 키를 기록했다)
class CEditPlayer :
    public CObj
{
private:
    bool m_bPause    = false;   // 멈추면 애니메이션을 진행하지 않는다 (에디터가 보여 줄 프레임을 정한다)
    Vec2 m_vGuide;              // 기준선: 가로선 반 길이, 기준점에서 가로선까지 (data\editor.ini [view] guide_size)
    bool m_bDocBelow = false;   // 편집 중인 것이 하체면 참고 애니메이션(상체) 아래에 그린다. 게임은 하체 위에 상체를 그린다

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void SetPause(const bool _bPause) { m_bPause = _bPause; }
    void SetDocBelow(const bool _bBelow) { m_bDocBelow = _bBelow; }

public:
    CLONE_DEACTIVATE(CEditPlayer);

public:
    CEditPlayer();
    virtual ~CEditPlayer() override;
};
