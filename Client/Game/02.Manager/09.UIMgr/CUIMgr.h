#pragma once

class CUI;

class CUIMgr
{
	SINGLE(CUIMgr);
private:
	CUI* m_pFocusedUI;
	CUI* m_pPriorityUI;

public:
	void tick();
	void Reset() { m_pFocusedUI = nullptr; m_pPriorityUI = nullptr; }	// 레벨을 바꿀 때 (이전 레벨의 UI를 가리키지 않게)

private:
	CUI* GetPriorityUI(CUI* _pParentUI) const;

};

