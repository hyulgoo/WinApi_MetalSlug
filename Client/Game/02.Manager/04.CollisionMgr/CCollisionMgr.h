#pragma once

class CCollider;
class CObj;

class CCollisionMgr
{
	SINGLE(CCollisionMgr);
private:
	// 지금 겹쳐 있는 충돌체 쌍. 겹침이 끝나거나 한쪽이 지워지면 뺀다
	struct tOverlap
	{
		CCollider* pLeft;
		CCollider* pRight;
	};

	// m_matrix[행 레이어]의 비트 = 검사할 열 레이어. 레이어를 늘리면 비트 수를 넘지 않는지 본다
	static_assert(static_cast<UINT>(LAYER::END) <= sizeof(WORD) * 8, "레이어가 충돌 행렬의 비트 수보다 많다");
	WORD                       m_matrix[static_cast<UINT>(LAYER::END)];
	map<ULONGLONG, tOverlap>   m_mapOverlap;		// 키: 두 충돌체 ID를 묶은 64비트 값

public:
	void LayerCheck(LAYER _left, LAYER _right);

	void Clear()
	{
		for (unsigned short& i : m_matrix)
            i = 0;
	}

	// 레벨을 바꿀 때: 검사할 레이어 쌍과 겹침 상태를 모두 비운다
	void Reset();

	// 오브젝트를 지울 때: 겹쳐 있던 상대와 EndOverlap을 주고받고 상태에서 뺀다
	void ReleaseObject(CObj* _pObj);

	size_t GetOverlapCount() const { return m_mapOverlap.size(); }

public:
	void tick();

private:
	void CollisionBtwLayer(LAYER _left, LAYER _right);
	bool CollisionBtwCollider(CCollider* _pLeft, CCollider* _pRight) const;

	static ULONGLONG MakeKey(UINT _iLeftID, UINT _iRightID);
};
