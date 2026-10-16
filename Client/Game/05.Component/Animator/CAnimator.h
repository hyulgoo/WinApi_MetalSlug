#pragma once
#include "Game\05.Component\CComponent.h"

class CAnimation;
class CTexture;

class CAnimator :
    public CComponent
{
private:
    map<wstring, CAnimation*>   m_mapAnim;// 애니메이션들을 보관하는 리스트(맵)
    CAnimation*                 m_pCurAnim; // 현재 재생 중인 애니메이션
    CAnimation*                 m_pMissingAnim = nullptr; // 없는 애니메이션을 재생하라고 하면 대신 도는 빈 애니메이션
    bool                        m_bRepeat; // 반복재생 여부

    // 색 바꿔 그리기 (피격 점멸): 오너 위치 기준 y [m_fTintTop, m_fTintBottom)에 드는 줄을 색을 바꾼 시트로 그린다
    bool                        m_bTint       = false;
    COLORREF                    m_tTintColor  = 0;
    float                       m_fTintTop    = 0.f;
    float                       m_fTintBottom = 0.f;

public:
    virtual void tick() override; // component에서 상속받은 가상함수를 재정의
    virtual void render(HDC _dc) override;
    virtual void final_tick() override;
    CAnimation*  GetAnimation() const { return m_pCurAnim; }
    bool         IsFinish() const;  // 재생 중인 애니메이션이 없거나 한 번 재생이 끝났다

    // 오너 위치 기준 y [_fTop, _fBottom)에 드는 줄을 _tColor로 바꿔 그린다 (애니메이션이 바뀌어도 ClearTint까지)
    void         SetTint(const COLORREF _tColor, const float _fTop, const float _fBottom)
    { m_bTint = true; m_tTintColor = _tColor; m_fTintTop = _fTop; m_fTintBottom = _fBottom; }
    void         ClearTint() { m_bTint = false; }
    bool         GetTint(COLORREF& _tColor, float& _fTop, float& _fBottom) const
    { _tColor = m_tTintColor; _fTop = m_fTintTop; _fBottom = m_fTintBottom; return m_bTint; }

public:
    CLONE(CAnimator);

public:
    void Play(const wstring& _strName, bool _bRepeat); // 재생할 애니메이션 strName, 반복 여부

    // 프레임을 직접 넘겨 애니메이션을 만든다 (애니메이션 에디터의 미리보기). 같은 이름이 있으면 바꾼다
    CAnimation* CreateFrameAnimation(const wstring& _strName, CTexture* _pAtlas, const vector<tAnimFrm>& _vecFrm);
    CAnimation* FindAnimation(const wstring& _strNamem); // strName에 해당하는 애니메이션 찾기
    CAnimation* LoadAnimation(const wstring& _strRelativePath);

    // 재생 중인 애니메이션이면 재생도 멈춘다 (GetAnimation()이 nullptr)
    void DeleteAnimation(const wstring& _strName);

public:
    CAnimator(CObj* _pOwner);
    CAnimator(const CAnimator& _other);
    ~CAnimator();
};
