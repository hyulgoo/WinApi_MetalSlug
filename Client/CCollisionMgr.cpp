#include "pch.h"
#include "CCollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CObj.h"
#include "CCollider.h"

CCollisionMgr::CCollisionMgr()
	: m_matrix{}
	, m_vCollisionArea{}
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::tick()
{
	for (UINT iRow = 0; iRow < (UINT)LAYER::END; ++iRow)
	{
		for (UINT iCol = iRow; iCol < (UINT)LAYER::END; ++iCol)
		{
			if (!(m_matrix[iRow] & (1 << iCol)) )			
				continue;
			
			// iRow 레이어와 iCol 레이어는 서로 충돌검사를 진행한다.
			CollisionBtwLayer((LAYER)iRow, (LAYER)iCol);
		}
	}
}

void CCollisionMgr::CollisionBtwLayer(LAYER _left, LAYER _right)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	const vector<CObj*>& vecLeft = pCurLevel->GetLayer(_left);
	const vector<CObj*>& vecRight = pCurLevel->GetLayer(_right);

	for (size_t i = 0; i < vecLeft.size(); ++i)
	{
		// 충돌체가 없는 경우
		if (nullptr == vecLeft[i]->GetCollider())
			continue;
				
		size_t j = 0;
		if (_left == _right) // Left, Right 동일 레이어인 경우, 이중 검사를 피하기 위함
		{
			j = i;
		}

		for (; j < vecRight.size(); ++j)
		{
			// 충돌체를 보유하고 있지 않거나, 충돌을 진행시킬 두 대상이 동일한 오브젝트인 경우
			if (nullptr == vecRight[j]->GetCollider() || vecLeft[i] == vecRight[j] )
				continue;

			// 두 충돌체의 아이디를 조합
			CollisionID ID = {};
			ID.LeftID = vecLeft[i]->GetCollider()->GetID();
			ID.RightID = vecRight[j]->GetCollider()->GetID();

			// 이전 프레임 충돌 확인
			map<UINT_PTR, bool>::iterator iter = m_mapPrevInfo.find(ID.id);
			if (iter == m_mapPrevInfo.end())
			{
				m_mapPrevInfo.insert(make_pair(ID.id, false));
				iter = m_mapPrevInfo.find(ID.id);
			}

			bool bDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();

			// 지금 겹쳐있다.
			if (CollisionBtwCollider(vecLeft[i]->GetCollider(), vecRight[j]->GetCollider()))
			{
				// 이전에도 겹쳐있었다.
				if (iter->second)
				{
					if (bDead)
					{
						vecLeft[i]->GetCollider()->EndOverlap(vecRight[j]->GetCollider());
						vecRight[j]->GetCollider()->EndOverlap(vecLeft[i]->GetCollider());
					}
					else
					{
						vecLeft[i]->GetCollider()->OnOverlap(vecRight[j]->GetCollider());
						vecRight[j]->GetCollider()->OnOverlap(vecLeft[i]->GetCollider());
					}					
				}

				// 이전에 충돌한 적이 없다.
				else
				{
					// 둘중 하나라도 Dead 상태면, 충돌을 없었던 것으로 한다.
					if (!bDead)
					{
						vecLeft[i]->GetCollider()->BeginOverlap(vecRight[j]->GetCollider());
						vecRight[j]->GetCollider()->BeginOverlap(vecLeft[i]->GetCollider());
						iter->second = NONE;
					}					
				}
			}

			// 지금 떨어져 있다.
			else
			{
				// 이전에는 겹쳐있었다.
				if (iter->second)
				{					
					vecLeft[i]->GetCollider()->EndOverlap(vecRight[j]->GetCollider());
					vecRight[j]->GetCollider()->EndOverlap(vecLeft[i]->GetCollider());
					iter->second = false;
				}				
			}			
		}
	}
}

bool CCollisionMgr::CollisionBtwCollider(CCollider* _pLeft, CCollider* _pRight)
{
	
	Vec2 vLeftPos = _pLeft->GetFinalPos();
	Vec2 vLeftScale = _pLeft->GetScale();

	Vec2 vRightPos = _pRight->GetFinalPos();
	Vec2 vRightScale = _pRight->GetScale();

	float VerticalStd = vLeftPos.x;

	if (fabsf(vLeftPos.x - vRightPos.x) > (vLeftScale.x / 2.f + vRightScale.x / 2.f))
	    return false;

	if (fabsf(vLeftPos.y - vRightPos.y) > (vLeftScale.y / 2.f + vRightScale.y / 2.f))
		return false;

	// float VerticalCollisionArea = (vLeftPos.x - vRightPos.x) - (vLeftScale.x / 2.f + vRightScale.x / 2.f);
	// float HorizonCollisionArea = (vLeftPos.y - vRightPos.y) - (vLeftScale.y / 2.f + vRightScale.y / 2.f);
	// 
	// Vec2 CollisionArea = Vec2(VerticalCollisionArea, HorizonCollisionArea);
	// 
	// Vec2 LeftOffset = _pLeft->GetOffsetPos();
	// Vec2 RightOffset = _pRight->GetOffsetPos();
	// _pLeft->SetOffsetPos(LeftOffset + CollisionArea);
	// _pRight->SetOffsetPos(RightOffset + CollisionArea);
	return true;
}

void CCollisionMgr::LayerCheck(LAYER _left, LAYER _right)
{
	UINT iRow = (UINT)_left;
	UINT iCol = (UINT)_right;

	if (iRow > iCol)
	{
		UINT iTemp = iCol;
		iCol = iRow;
		iRow = iTemp;
	}

	m_matrix[iRow] |= (1 << iCol);
}
