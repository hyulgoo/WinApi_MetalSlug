#pragma once
#include "Game\05.Component\CComponent.h"

// 픽셀 충돌(대표 기능)
// 레벨이 넘겨준 충돌 맵(배경 텍스처 좌표)을 오너의 월드 위치로 바로 조회해, 발이 땅(마젠타)에 묻히면 위로,
// 옆이 벽(초록)에 묻히면 반대쪽으로 밀어낸다. 아래로 떨어지는 움직임(중력·등속 낙하)은 Rigidbody가 맡는다.
// 예전에는 오브젝트마다 매 tick 배경 충돌 맵을 화면 크기로 확대해 공유 텍스처에 다시 그리고 거기서 읽었다
class CPixelCollider :
    public CComponent
{
private:
    Vec2        m_vOffsetPos;
    bool        m_bShowMap;   // 디버그: 충돌 맵을 화면에 겹쳐 그린다 (스테이지 R 키)
    bool        m_bJump;      // 발밑 10px 안에 땅이 없다 (공중)
    bool        m_bGround;
    bool        m_bWall;

public:
    void SetOffsetPos(const Vec2& _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    Vec2 GetOffsetPos() const { return m_vOffsetPos; }
    bool IsJump() const { return m_bJump; }
    bool IsGround() const { return m_bGround; }
    bool IsWall() const { return m_bWall; }
    void ToggleShowMap() { m_bShowMap = !m_bShowMap; }

public:
    virtual void tick() override;
    virtual void final_tick() override;
    virtual void render(HDC _dc) override;

    CLONE_DEACTIVATE(CPixelCollider);

public:
    CPixelCollider(CObj* _pOwner);
    ~CPixelCollider();
};
