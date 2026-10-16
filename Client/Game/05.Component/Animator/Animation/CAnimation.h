#pragma once
#include "Game\Entity\CEntity.h"

class CAnimator;
class CTexture;
class CAnimData;

class CAnimation :
    public CEntity
{
private:
    CAnimator*        m_pAnimator; // Obj에서 만들어주는 CreateAnimator
    const CAnimData*  m_pData = nullptr;   // 파일에서 읽은 공유 프레임 (있으면 이것을 읽는다)
    vector<tAnimFrm>  m_vecFrm;   // 이 애니메이션만의 프레임 (에디터가 만든 것)
    CTexture*         m_pAtlas;
    int               m_iCurFrm;
    float             m_fAccTime;
    bool              m_bFinish;
    CTexture*         m_pBackBuffer;
    float             m_fmagni;
    bool              m_bReverse;
    SETPRINT          m_tPrintMode;
    Vec2              m_vUpperOffset;   // 상체(UPPER)·하체(LOWER)로 그릴 때 더 옮기는 거리 (시트 px)
    Vec2              m_vLowerOffset;
    bool              m_bClip       = false;   // 아래를 잘라 그린다 (땅에서 솟아오르는 보스)
    float             m_fClipBottom = 0.f;     // 이 월드 y보다 아래는 그리지 않는다

    // 이번 tick에 지나간 프레임 (프레임 이벤트 판정용)
    int               m_iTickStartFrm = 0;   // tick을 시작할 때의 프레임
    int               m_iTickLoops    = 0;   // tick 동안 처음으로 되돌아간 횟수

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
    tAnimFrm GetCurFrm() const { return Frms().empty() ? tAnimFrm{} : Frms()[m_iCurFrm]; }
    // _iFrm번 프레임 정보 (재생 상태는 바꾸지 않는다. 범위 밖이면 가장 가까운 프레임)
    tAnimFrm GetFrm(const int _iFrm) const { return Frms().empty() ? tAnimFrm{} : Frms()[max(0, min(_iFrm, GetFrmCount() - 1))]; }
    int      GetCurFrmNumber() const { return m_iCurFrm; }
    int      GetFrmCount() const { return static_cast<int>(Frms().size()); }
    // 프레임을 통째로 바꾼다 (에디터 미리보기). 공유 데이터는 더 보지 않고, 현재 프레임은 새 범위 안으로
    void     SetAllFrmInfo(const vector<tAnimFrm>& _Other);
    // 시트 그림 그대로 그린다 / 좌우로 뒤집어 그린다. 플레이어 시트는 오른쪽을, 적 시트는 대부분 왼쪽을 보므로
    // "오른쪽을 보게"가 아니라 "그대로·뒤집어"로 읽는다 (적은 CMonster::ApplyFacing이 그림 방향을 보고 고른다)
    void     SetPlayRight() { m_bReverse = false; }
    void     SetPlayLeft() { m_bReverse = true; }
    void     SetUpper() { m_tPrintMode = SETPRINT::UPPER;}
    void     SetLower() { m_tPrintMode = SETPRINT::LOWER; }
    void     SetPrintMode(const SETPRINT _tMode) { m_tPrintMode = _tMode; }
    void     SetFrmNum(const int _Num) { m_iCurFrm = Frms().empty() ? 0 : max(0, min(_Num, GetFrmCount() - 1)); }
    // 월드 y _fWorldY보다 아래는 그리지 않는다 (땅 밑에서 올라오는 그림). ClearClip으로 끈다
    void     SetClipBottom(const float _fWorldY) { m_bClip = true; m_fClipBottom = _fWorldY; }
    void     ClearClip() { m_bClip = false; }

    void      SetAtlas(CTexture* _pAtlas) { m_pAtlas = _pAtlas; }
    CTexture* GetAtlas() const { return m_pAtlas; }
    void SetMagni(const float _fmagni) { m_fmagni = _fmagni; }

    // 파일에서 읽은 공유 데이터를 쓴다 (이름·아틀라스도 데이터를 따른다)
    void SetData(const CAnimData* _pData);

private:
    // 지금 쓰는 프레임 (공유 데이터 또는 자기 프레임)
    const vector<tAnimFrm>& Frms() const;
    // 애니메이터가 색 바꿔 그리기를 켰으면, 확대 버퍼에 옮긴 프레임 중 그 높이에 드는 줄을 색을 바꾼 시트로 덮는다
    // (_fFrameTopY: 프레임 위 끝, _fOwnerY: 오너 위치. 둘 다 화면 y)
    void DrawTintRows(const tAnimFrm& _frm, float _fFrameTopY, float _fOwnerY);

public:
    CLONE(CAnimation);

public:
    CAnimation(CAnimator* _pAnimator);
    ~CAnimation();

    friend class CAnimator;
};
