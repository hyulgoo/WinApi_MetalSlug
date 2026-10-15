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

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CStartLevel();
    virtual ~CStartLevel() override;

};

