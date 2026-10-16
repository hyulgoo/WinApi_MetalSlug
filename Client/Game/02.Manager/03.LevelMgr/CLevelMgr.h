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
	// 다음에 들어갈 레벨에 미리 알릴 것이 있을 때 (타이틀 -> 보스전부터 시작)
	CLevel*    GetLevel(const LEVEL_TYPE _eType) const { return m_arrLevel[static_cast<UINT>(_eType)]; }

	friend class CEventMgr;
};
