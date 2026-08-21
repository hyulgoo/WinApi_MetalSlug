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
    bool                        m_bRepeat; // 반복재생 여부
    bool                        m_bLink; // 연결된 애니메이션 여부
    wstring                     m_strLinkName;
    SETPRINT                    m_tLinkPrint;
    bool                        m_tLinkDir;

public:
    virtual void tick() override; // component에서 상속받은 가상함수를 재정의
    virtual void render(HDC _dc) override;
    virtual void final_tick() override;
    void         SetLink(const wstring& _strLinkName, SETPRINT _tUpDownNormal, bool _bDirection);
    void         SetLinkAnim(SETPRINT _tUpDownNormal, bool _bDirection);
    CAnimation*  GetAnimation() const { return m_pCurAnim; }

public:
    CLONE(CAnimator);

public:
    void Play(const wstring& _strName, bool _bRepeat); // 재생할 애니메이션 strName, 반복 여부

    // 애니메이션 생성, 애니메이션 이름, 아틀라스 텍스처, 자르는 시작 위치, 사이즈, 총 개수, 1프레임당 지속시간
    void CreateAnimation(const wstring& _strName, CTexture* _pAtlas, const Vec2& _vLeftTop, const Vec2& _vSize
                        , const Vec2& _vOffset, int _iMaxFrmCount, float _FDuration);
    void CreateEditUpperAnimation(const vector<tAnimFrm>& _vecFrm, CTexture* _Atlas);
    void CreateEditLowerAnimation(const vector<tAnimFrm>& _vecFrm, CTexture* _Atlas);
    CAnimation* FindAnimation(const wstring& _strNamem); // strName에 해당하는 애니메이션 찾기
    CAnimation* LoadAnimation(const wstring& _strRelativePath);

    void DeleteAnimation(const wstring& _strName);

public:
    CAnimator(CObj* _pOwner);
    CAnimator(const CAnimator& _other);
    ~CAnimator();
};

