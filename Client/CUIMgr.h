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

private:
	CUI* GetPriorityUI(CUI* _pParentUI);

};

