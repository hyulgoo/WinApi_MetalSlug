#pragma once

#include "CLevel.h"

class CSeletAnim;
class CTexture;
class CObj;

class CStartLevel :
    public CLevel
{
private:
    bool        m_bStart;
    CTexture*   m_pAtlas;
    CSeletAnim* m_pSelectAnim;
    float       m_fTime;
    bool        m_bEnd;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CStartLevel();
    ~CStartLevel();

};

