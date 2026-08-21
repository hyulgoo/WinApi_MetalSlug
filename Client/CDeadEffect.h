#pragma once
#include "CObj.h"

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

    void LoadAnimation();
    void PlayAnim(const wstring& _strName, bool _bRepeat, bool _bDirRight);
    void SetFire() { m_bFire = true; }
    void SetType(int _iType) { m_iType = _iType; }
    void SetDir(bool _bDir) { m_bDir = _bDir; }
    void Play(bool _bDir);
    LAYER GetLayer() { return LAYER::DEFAULT; }
public:
    CDeadEffect();
    ~CDeadEffect();
};

