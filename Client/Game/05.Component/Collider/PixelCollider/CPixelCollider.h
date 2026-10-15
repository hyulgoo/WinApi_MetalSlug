#pragma once
#include "Game\05.Component\CComponent.h"

// 픽셀 충돌(대표 기능)
// 레벨이 넘겨준 충돌 맵(배경 텍스처 좌표)을 오너의 월드 위치로 바로 조회해, 발이 땅(마젠타)에 묻히면 위로,
// 옆이 벽(초록)에 묻히면 반대쪽으로 밀어낸다. 아래로 떨어지는 움직임(중력·등속 낙하)은 Rigidbody가 맡는다.
// 예전에는 오브젝트마다 매 tick 배경 충돌 맵을 화면 크기로 확대해 공유 텍스처에 다시 그리고 거기서 읽었다.
// 맵 위에 얹힌 발판(OBSTACLE 레이어, 보스가 떨어뜨린 비석)은 위에서 내려올 때만 밟는다 (옆·아래에서는 지나간다)
class CPixelCollider :
    public CComponent
{
private:
    Vec2        m_vOffsetPos;
    bool        m_bShowMap;   // 디버그: 충돌 맵을 화면에 겹쳐 그린다 (스테이지 R 키)
    bool        m_bAirborne;      // 발밑 m_fGroundProbe 안에 땅이 없다 (공중)
    float       m_fGroundProbe;   // data\game.ini [pixel_collider] ground_probe
    float       m_fPlatformBand;  // [pixel_collider] platform_band: 발판 윗면에서 이만큼 아래까지 발이 들어오면 윗면에 올린다
    float       m_fPlatformStick; // [pixel_collider] platform_stick: 발판에 서 있었으면 윗면이 이만큼 내려가도 따라 내려간다
    bool        m_bGround;
    bool        m_bWall;
    bool        m_bOnPlatform;    // 지난 tick에 발판 위에 섰다
    bool        m_bHasPrevFoot;   // 지난 tick의 발 높이가 있다
    float       m_fPrevFootY;     // 지난 tick이 끝났을 때 발 높이 (월드 y). 이보다 위로 갔으면 올라가는 중

public:
    // 월드 x에서 _fFromY부터 아래로 내려가며 처음 만나는 땅의 윗면(월드 y). 지금 레벨에 충돌 맵이 없거나 땅이 없으면 false
    static bool FindGroundBelow(float _fX, float _fFromY, float& _fGroundY);

public:
    void SetOffsetPos(const Vec2& _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    Vec2 GetOffsetPos() const { return m_vOffsetPos; }
    bool IsAirborne() const { return m_bAirborne; }
    bool IsGround() const { return m_bGround; }
    bool IsWall() const { return m_bWall; }
    void ToggleShowMap() { m_bShowMap = !m_bShowMap; }

private:
    // 발이 들어온 발판의 윗면 (여럿이면 가장 높은 것). 위로 올라가는 중이면 밟지 않는다
    bool FindPlatform(const Vec2& _vFoot, float& _fTop) const;
    void StorePrevFoot();

public:
    virtual void tick() override;
    virtual void final_tick() override;
    virtual void render(HDC _dc) override;

    CLONE_DEACTIVATE(CPixelCollider);

public:
    CPixelCollider(CObj* _pOwner);
    ~CPixelCollider();
};
