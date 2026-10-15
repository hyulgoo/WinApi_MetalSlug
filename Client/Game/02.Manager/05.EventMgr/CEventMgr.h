#pragma once

class CObj;

// 지연처리 업무 담당
// 오브젝트 생성·삭제, 레벨 전환, AI 상태 변경은 프레임 중에 요청만 쌓고 프레임 끝에 한꺼번에 처리한다.
// - 삭제: 요청을 처리할 때 삭제 표시를 하고 레이어에서 뺀다. 메모리는 다음 프레임 처리 때 해제한다
//   (그 한 프레임 동안 다른 오브젝트가 들고 있던 포인터로 IsDead()를 확인할 수 있다)
// - 레벨 전환: 이번 프레임의 요청을 모두 처리한 뒤 프레임 경계에서 한다
// - 처리 도중 새로 들어온 요청은 다음 프레임에 처리한다
class CEventMgr
{
	SINGLE(CEventMgr);
private:
	vector<tEvent>	m_vecEvent;
	vector<CObj*>	m_vecGarbage;

public:
	void AddEvent(const tEvent& _event) { m_vecEvent.push_back(_event); }

	void tick();

private:
	void DeleteGarbage();
	static void DiscardEvents(vector<tEvent>& _vecEvent);
};
