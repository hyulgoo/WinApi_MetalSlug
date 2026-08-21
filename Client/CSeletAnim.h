#pragma once
#include "CObj.h"
class CSeletAnim :
    public CObj
{
private:
    CTexture* m_pAtlas;
    CTexture* m_pBackBuffer;
    bool      m_bStart;
    Vec2      m_vLeftTop;
    Vec2      m_vSize;
    Vec2      m_fOffset;
    int       m_iCurFrm;
    float     m_fTime;
    float     m_fmagni;
    bool      m_bPrev;

public:
    virtual void render(HDC _dc) override;
    void SetStart() { m_bStart = true; }
    void SetNumber() { m_iCurFrm = 1; }

public:
    CSeletAnim();
    ~CSeletAnim();

};

