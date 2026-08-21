#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CDeadEffect :
    public CObj
{
private:
    CTexture*   m_pAtlas;
    bool        m_bDir;
    float       m_fDeadTime;
    float       m_fTime;
    bool        m_bFire;
    int         m_iType;
    int         m_irand;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void          LoadAnimation();
    void          PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
    void          SetFire() { m_bFire = true; }
    void          SetType(const int _iType) { m_iType = _iType; }
    void          SetDir(const bool _bDir) { m_bDir = _bDir; }
    void          Play(bool _bDir);
    virtual LAYER GetLayer() override { return LAYER::DEFAULT; }
public:
    CDeadEffect();
    virtual ~CDeadEffect() override;
};

