#pragma once

class CLevel;

class CLevelMgr
{
	SINGLE(CLevelMgr)
private:
	CLevel*    m_arrLevel[static_cast<UINT>(LEVEL_TYPE::END)];
	CLevel*    m_pCurLevel;
	LEVEL_TYPE m_eCurLevel;

public:
	void init();
	void tick() const;
	void render(HDC _dc) const;
private:
	void ChangeLevel(LEVEL_TYPE _eNext);

public:
	CLevel*    GetCurLevel() const { return m_pCurLevel; }
	LEVEL_TYPE GetCurLevelType() const { return m_eCurLevel; }

	friend class CEventMgr;
};
