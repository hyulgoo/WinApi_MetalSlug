#pragma once

#include "Game\03.Level\CLevel.h"

class CSelectAnim;
class CTexture;
class CObj;

class CStartLevel :
    public CLevel
{
private:
    bool        m_bStart;
    CSelectAnim* m_pSelectAnim;
    float       m_fTime;
    bool        m_bEnd;
    bool        m_bBossStart;   // F2로 골랐다: 스테이지에 보스전부터 시작하라고 알린다

    // 수치 (data\title.ini [title]). 들어올 때마다 읽는다
    struct tData
    {
        float    fShakeRange  = 0.f;
        float    fShakeSpeed  = 0.f;
        float    fShakeTime   = 0.f;
        float    fFadeOutAt   = 0.f;
        float    fFadeOutTime = 0.f;
        float    fStageAt     = 0.f;
        Vec2     vHelpPos;
        COLORREF tHelpColor   = 0;
    };
    tData       m_tData;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CStartLevel();
    virtual ~CStartLevel() override;

};

