#pragma once


class CEntity
{
private:
	static UINT	g_iNextID;

private:	
	wstring		m_strName;
	const UINT	m_id;

public:
	void SetName(const wstring& _str) {	m_strName = _str; }
	const wstring& GetName() { return m_strName; }
	UINT GetID() const { return m_id; }

	virtual CEntity* Clone() = 0;

public:
	CEntity();
	CEntity(const CEntity& _Origin);
	~CEntity();
};

