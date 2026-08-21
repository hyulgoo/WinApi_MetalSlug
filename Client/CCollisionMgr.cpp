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
    for (UINT iRow = 0; iRow < static_cast<UINT>(LAYER::END); ++iRow)
    {
        for (UINT iCol = iRow; iCol < static_cast<UINT>(LAYER::END); ++iCol)
        {
            if (!(m_matrix[iRow] & (1 << iCol)))
                continue;

            // iRow 레이어와 iCol 레이어 간의 충돌검사를 수행한다.
            CollisionBtwLayer(static_cast<LAYER>(iRow), static_cast<LAYER>(iCol));
        }
    }
}

void CCollisionMgr::CollisionBtwLayer(const LAYER _left, const LAYER _right)
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

    const vector<CObj*>& vecLeft  = pCurLevel->GetLayer(_left);
    const vector<CObj*>& vecRight = pCurLevel->GetLayer(_right);

    const size_t leftSize  = vecLeft.size();
    const size_t rightSize = vecRight.size();
    for (size_t i = 0; i < leftSize; ++i)
    {
        // 충돌체가 없는 경우
        if (nullptr == vecLeft[i]->GetCollider())
            continue;

        size_t j = 0;
        if (_left == _right) // Left, Right 가 같은 레이어인 경우, 중복 검사를 피하기 위함
            j = i;
        
        for (; j < rightSize; ++j)
        {
            // 충돌체가 존재하지 않거나, 자기 자신과 비교하는 경우
            if (nullptr == vecRight[j]->GetCollider() || vecLeft[i] == vecRight[j])
                continue;

            // 두 충돌체의 아이디 저장
            CollisionID ID = {};
            ID.LeftID      = vecLeft[i]->GetCollider()->GetID();
            ID.RightID     = vecRight[j]->GetCollider()->GetID();

            // 이전 프레임의 충돌 확인
            map<UINT_PTR, bool>::iterator iter = m_mapPrevInfo.find(ID.id);
            if (iter == m_mapPrevInfo.end())
            {
                m_mapPrevInfo.insert(make_pair(ID.id, false));
                iter = m_mapPrevInfo.find(ID.id);
            }

            const bool bDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();

            // 지금 충돌해 있다.
            if (CollisionBtwCollider(vecLeft[i]->GetCollider(), vecRight[j]->GetCollider()))
            {
                // 이전에도 충돌해 있었다.
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
                    // 둘 중 하나라도 Dead 상태면, 충돌을 시작하지 않게 한다.
                    if (!bDead)
                    {
                        vecLeft[i]->GetCollider()->BeginOverlap(vecRight[j]->GetCollider());
                        vecRight[j]->GetCollider()->BeginOverlap(vecLeft[i]->GetCollider());
                        iter->second = NONE;
                    }
                }
            }
            // 지금은 충돌해 있지 않다.
            else
            {
                // 이전에는 충돌해 있었다.
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

bool CCollisionMgr::CollisionBtwCollider(CCollider* _pLeft, CCollider* _pRight) const
{
    const Vec2 vLeftPos   = _pLeft->GetFinalPos();
    const Vec2 vLeftScale = _pLeft->GetScale();

    const Vec2 vRightPos   = _pRight->GetFinalPos();
    const Vec2 vRightScale = _pRight->GetScale();


    if (fabsf(vLeftPos.x - vRightPos.x) > (vLeftScale.x / 2.f + vRightScale.x / 2.f))
        return false;

    if (fabsf(vLeftPos.y - vRightPos.y) > (vLeftScale.y / 2.f + vRightScale.y / 2.f))
        return false;

    //float VerticalStd = vLeftPos.x;
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
    UINT iRow = static_cast<UINT>(_left);
    UINT iCol = static_cast<UINT>(_right);

    if (iRow > iCol)
    {
        const UINT iTemp = iCol;
        iCol             = iRow;
        iRow             = iTemp;
    }

    m_matrix[iRow] |= (1 << iCol);
}
