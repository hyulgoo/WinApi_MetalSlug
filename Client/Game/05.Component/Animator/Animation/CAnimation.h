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

    // 이번 tick에 지나간 프레임 (프레임 이벤트 판정용)
    int               m_iTickStartFrm = 0;   // tick을 시작할 때의 프레임
    int               m_iTickLoops    = 0;   // tick 동안 처음으로 되돌아간 횟수

private:
    void init(const wstring& _strName, CTexture* _pAtlas, const Vec2& _vLeftTop, const Vec2& _vSize, const Vec2& _vOffset, int _iMaxFrmCount, float _fDuration);

public:
    // 남은 시간을 다음 프레임으로 넘기며 진행한다. DT가 길면 한 tick에 여러 프레임을 넘긴다.
    // _bRepeat이면 끝에서 처음으로 되돌아가고(남은 시간도 이어서) 끝나지 않는다
    void tick(bool _bRepeat);
    void render(HDC _dc);
    bool IsFinish() const { return m_bFinish; }
    // 이번 tick에 _iFrm 프레임에 들어섰는가. 한 tick에 여러 프레임을 건너뛰어도 빠뜨리지 않는다
    // (Play/Reset으로 0번에서 시작한 것은 들어선 것으로 치지 않는다)
    bool IsFrameEntered(int _iFrm) const;
    void Reset()
    { // 재생 종료 상태를 false로 만들고 프레임과 누적 시간을 초기화한다.
        m_bFinish       = false;
        m_iCurFrm       = 0;
        m_fAccTime      = 0.f;
        m_iTickStartFrm = 0;
        m_iTickLoops    = 0;
    }
    tAnimFrm GetCurFrm() const { return m_vecFrm.empty() ? tAnimFrm{} : m_vecFrm[m_iCurFrm]; }
    int      GetCurFrmNumber() const { return m_iCurFrm; }
    int      GetFrmCount() const { return static_cast<int>(m_vecFrm.size()); }
    void     NextFrm();
    void     PrevFrm();
    void     FirstFrm() { m_iCurFrm = 0; }
    void     SetCurFrmInfo(const tAnimFrm& _Other);
    void     SetAllFrmInfo(const vector<tAnimFrm>& _Other);
    void     SetPlayRight() { m_bReverse = false; }
    void     SetPlayLeft() { m_bReverse = true; }
    void     SetUpper() { m_tPrintMode = SETPRINT::UPPER;}
    void     SetLower() { m_tPrintMode = SETPRINT::LOWER; }
    void     SetFrmNum(const int _Num) { m_iCurFrm = m_vecFrm.empty() ? 0 : max(0, min(_Num, GetFrmCount() - 1)); }

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
