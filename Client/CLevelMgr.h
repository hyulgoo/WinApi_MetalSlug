#pragma once

class CLevel;

class CLevelMgr
{
	SINGLE(CLevelMgr);
private:
	CLevel* m_arrLevel[(UINT)LEVEL_TYPE::END];
	CLevel* m_pCurLevel;

public:
	void init();
	void tick();
	void render(HDC _dc);
private:
	void ChangeLevel(LEVEL_TYPE _eNext);

public:
	CLevel* GetCurLevel() { return m_pCurLevel; }

	friend class CEventMgr;
};

