#pragma once
#include "Game\04.Object\UI\CUI.h"

#include <functional>

// 애니메이션 에디터 패널의 글자 버튼. 누르면 SetOnClick으로 받은 함수를 부른다
// (예전에는 파생 클래스의 const 멤버 함수 포인터를 reinterpret_cast로 void (CEntity::*)()에 넣어 불렀다.
//  표준상 정의되지 않은 동작이고 MSVC 단일 상속에서만 우연히 동작했다, MS-R44)
class CEditorButton :
    public CUI
{
private:
    std::function<void()> m_fnClick;
    wstring               m_strText;

    // 색 (data\editor.ini [button]): 보통, 마우스가 올라갔을 때, 누르고 있을 때
    COLORREF              m_tColor      = 0;
    COLORREF              m_tHoverColor = 0;
    COLORREF              m_tPressColor = 0;

public:
    void SetOnClick(std::function<void()> _fnClick) { m_fnClick = std::move(_fnClick); }
    void SetText(const wstring& _strText) { m_strText = _strText; }

    virtual void render(HDC _dc) override;

    CLONE(CEditorButton);

private:
    virtual void MouseLbtnClicked() override;

public:
    CEditorButton();
    virtual ~CEditorButton() override;
};
