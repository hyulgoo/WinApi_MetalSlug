#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

// 캐릭터 앞에 그려지는 배경 조각(스테이지에 두 개). 월드 좌표에 놓인다
class CBackGround_Front :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pScaled;     // 아틀라스를 BG_MAGNI배로 미리 확대해 둔 것 (예전에는 매 프레임 확대했다)
    Vec2      m_vecResolution;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

public:
    CLONE_DEACTIVATE(CBackGround_Front);

public:
    // _strKey·_strPath: 조각 텍스처, _vPos: 월드 위치(왼쪽 위)
    CBackGround_Front(const wstring& _strKey, const wstring& _strPath, const Vec2& _vPos);
    CBackGround_Front(const CBackGround_Front&) = delete;
    virtual ~CBackGround_Front() override;
};
