#pragma once

#include "Game\03.Level\CLevel.h"

class CTexture;
class CObj;

class CGameOver :
    public CLevel
{
private:
    CTexture* m_pAtlas;
    Vec2      m_vResolution;
    float     m_fTime;
    float     m_fHoldTime;   // C를 이만큼 누르고 있으면 타이틀로 (data\title.ini [gameover])

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CGameOver();
    virtual ~CGameOver() override;
};
