#pragma once

class CObj;

// 지연처리 업무 담당
class CEventMgr
{
	SINGLE(CEventMgr);
private:
	vector<tEvent>	m_vecEvent;
	vector<CObj*>	m_vecGarbage;

public:
	void AddEvent(const tEvent& _event) { m_vecEvent.push_back(_event); }

	void tick();
};

 