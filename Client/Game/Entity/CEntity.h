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
	// 파생 객체를 기반 포인터로 지우는 계층(CState를 CAI가 지우는 등)이 있으므로 가상이어야 한다
	virtual ~CEntity();
};

