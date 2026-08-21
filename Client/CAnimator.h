#pragma once
#include "CComponent.h"

class CAnimation;
class CTexture;

class CAnimator :
    public CComponent
{
private:
    map<wstring, CAnimation*>   m_mapAnim;// 애니메이션을 담고 있을 이진트리 배열
    CAnimation*                 m_pCurAnim; // 현재 재생될 애니메이션
    bool                        m_bRepeat; // 반복재생 여부
    bool                        m_bLink; // 연계할 애니메이션 여부
    wstring                     m_strLinkName;
    SETPRINT                    m_tLinkPrint;
    bool                        m_tLinkDir;

public:
    virtual void tick() override; // component에서 받은 순수가상함수를 구현
    virtual void render(HDC _dc) override;
    virtual void final_tick();
    void SetLink(const wstring& _strLinkName, SETPRINT _tUpDownNormal, bool _bDirection);
    void SetLinkAnim(SETPRINT _tUpDownNormal, bool _bDirection);
    CAnimation* GetAnimation() { return m_pCurAnim; }

public:
    CLONE(CAnimator);

public:
    void Play(const wstring& _strName, bool _bRepeat); // 재생할 애니메이션 strName, 반복 여부

    // 애니메이션 생성, 애니메이션 이름, 뽑아올 텍스쳐, 자르기 시작할 위치, 사이즈, 장 수, 1프레임당 지속시간
    void CreateAnimation(const wstring& _strName, CTexture* _pAtlas, Vec2 _vLeftTop, Vec2 _vSize
                        , Vec2 _vOffset, int _iMaxFrmCount, float _FDuration);
    void CreateEditUpperAnimation(vector<tAnimFrm> _vecFrm, CTexture* _Atlas);
    void CreateEditLowerAnimation(vector<tAnimFrm> _vecFrm, CTexture* _Atlas);
    CAnimation* FindAnimation(const wstring& _strNamem); // strName을 가지고 애니메이션 찾기
    CAnimation* LoadAnimaton(wstring _strRelativePath);

    void DeleteAnimation(const wstring& _strName);

public:
    CAnimator(CObj* _pOwner);
    CAnimator(const CAnimator& _other);
    ~CAnimator();
};

