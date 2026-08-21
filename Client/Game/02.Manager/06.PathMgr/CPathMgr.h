#pragma once

class CPathMgr
{
	SINGLE(CPathMgr)
private:
	wchar_t		m_szContentPath[256];

public:
	void init();
	void render() const;

public:
	const wchar_t* GetContentPath() const { return m_szContentPath; }
};

