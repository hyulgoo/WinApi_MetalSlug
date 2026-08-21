#pragma once
#include "CObj.h"

class CTexture;

class CPlayerCQC :
    public CObj
{
private:
    float m_fSpeed;
    float m_fDegree;
    float m_fTime;

    CObj*     m_pTargetObject;
    CTexture* m_pTexture;
    CTexture* m_pBackBuffer;
    bool      m_bStart;

public:
    void SetSpeed(const float _f) { m_fSpeed = _f; }
    void SetDir(const float _fDegree) { m_fDegree = _fDegree; }

public:
    virtual LAYER GetLayer() override { return LAYER::CQC; }

public:
    virtual void tick() override;
    virtual void BeginOverlap(CCollider* _pOther) override;

public:
    CPlayerCQC();
    virtual ~CPlayerCQC() override;
};
