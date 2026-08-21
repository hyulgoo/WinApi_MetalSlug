#pragma once

#include "Game\03.Level\CLevel.h"

class CSeletAnim;
class CTexture;
class CObj;

class CGameOver :
    public CLevel
{
private:
    CTexture* m_pAtlas;
    Vec2      m_vResolution;
    float     m_fTime;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CGameOver();
    virtual ~CGameOver() override;
};
