#pragma once
#include "Game\Entity\CEntity.h"

class CAnimator;
class CTexture;

class CAnimation :
    public CEntity
{
private:
    CAnimator*        m_pAnimator; // Obj에서 만들어주는 CreateAnimator
    vector<tAnimFrm>  m_vecFrm;   // Animation 재생에 필요한 프레임 정보가 담긴 vector
    CTexture*         m_pAtlas;
    int               m_iCurFrm;
    float             m_fAccTime;
    bool              m_bFinish;
    CTexture*         m_pBackBuffer;
    float             m_fmagni;
    bool              m_bReverse;
    Vec2              m_vOffset;
    SETPRINT          m_tPrintMode;

private:
    void init(const wstring& _strName, CTexture* _pAtlas, const Vec2& _vLeftTop, const Vec2& _vSize, const Vec2& _vOffset, int _iMaxFrmCount, float _fDuration);

public:
    void tick();
    void render(HDC _dc);
    bool IsFinish() const { return m_bFinish; }
    void Reset()
    { // 재생 종료 상태를 false로 만들고 프레임과 누적 시간을 초기화한다.
        m_bFinish = false;
        m_iCurFrm = 0;
        m_fAccTime = 0.f;
    }
    tAnimFrm GetCurFrm() { return m_vecFrm[m_iCurFrm]; }
    int      GetCurFrmNumber() const { return m_iCurFrm; }
    void     NextFrm();
    void     PrevFrm();
    void     FirstFrm() { m_iCurFrm = 0; }
    void     SetCurFrmInfo(const tAnimFrm& _Other);
    void     SetAllFrmInfo(const vector<tAnimFrm>& _Other);
    void     SetPlayRight() { m_bReverse = false; }
    void     SetPlayLeft() { m_bReverse = true; }
    void     SetUpper() { m_tPrintMode = SETPRINT::UPPER;}
    void     SetLower() { m_tPrintMode = SETPRINT::LOWER; }
    void     SetFrmNum(const int _Num) { m_iCurFrm = _Num; }

    vector<tAnimFrm> GetFrmInfo() { return m_vecFrm; }
    void SetAtlas(CTexture* _pAtlas) { m_pAtlas = _pAtlas; }
    void SetMagni(const float _fmagni) { m_fmagni = _fmagni; }

    void Save(const wstring& _strRelativePath);
    void Load(const wstring& _strRelativePath);

public:
    CLONE(CAnimation);

public:
    CAnimation(CAnimator* _pAnimator);
    ~CAnimation();

    friend class CAnimator;
};
