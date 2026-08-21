#pragma once

class CCollider;

//#define MAKE_COLID(left, right) (left << 32) | right
enum CollisionDir
{
	UP,
	DOWM,
	LEFT,
	RIGHT,
	NONE

};

union CollisionID
{
	struct
	{
		UINT LeftID;
		UINT RightID;
	};

	UINT_PTR id;
};


class CCollisionMgr
{
	SINGLE(CCollisionMgr);
private:
	WORD					m_matrix[(UINT)LAYER::END];
	map<UINT_PTR, bool>		m_mapPrevInfo;		// 이전 프레임에 두 충돌체의 충돌여부
	Vec2 m_vCollisionArea;

public:
	void LayerCheck(LAYER _left, LAYER _right);

	void Clear()
	{
		for (int i = 0; i < (UINT)LAYER::END; ++i)
			m_matrix[i] = 0;
	}

public:
	void tick();

private:
	void CollisionBtwLayer(LAYER _left, LAYER _right);
	bool CollisionBtwCollider(CCollider* _pLeft, CCollider* _pRight);

};

