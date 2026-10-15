#pragma once
#include "Game\04.Object\CObj.h"

class CTexture;

class CDeadEffect :
    public CObj
{
private:
    bool        m_bDir;
    bool        m_bStart;
    bool        m_bFire;
    int         m_iType;
    int         m_irand;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;

    void          LoadAnimation();
    // 사망 그림은 모두 왼쪽을 본다. 몬스터가 오른쪽을 보고 죽었으면 좌우로 뒤집어 그린다
    void          PlayAnim(const wstring& _strName, bool _bRepeat, bool _bFacesRight);
    void          SetFire() { m_bFire = true; }
    void          SetType(const int _iType) { m_iType = _iType; }
    void          SetDir(const bool _bDir) { m_bDir = _bDir; }
    void          Play(bool _bFacesRight);
public:
    CDeadEffect();
    virtual ~CDeadEffect() override;
};

