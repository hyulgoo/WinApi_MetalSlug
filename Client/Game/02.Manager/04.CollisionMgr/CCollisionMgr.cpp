#include "pch.h"
#include "Game\02.Manager\04.CollisionMgr\CCollisionMgr.h"

#include "Game\02.Manager\03.LevelMgr\CLevelMgr.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\CObj.h"
#include "Game\05.Component\Collider\CCollider.h"

CCollisionMgr::CCollisionMgr()
    : m_matrix{}
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

ULONGLONG CCollisionMgr::MakeKey(const UINT _iLeftID, const UINT _iRightID)
{
    // 두 ID를 32비트씩 묶는다. 작은 ID를 위로 두어 순서와 상관없이 같은 키가 된다
    const UINT iLow  = min(_iLeftID, _iRightID);
    const UINT iHigh = max(_iLeftID, _iRightID);
    return (static_cast<ULONGLONG>(iLow) << 32) | iHigh;
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
        CCollider* pLeft = vecLeft[i]->GetCollider();
        if (nullptr == pLeft)
            continue;

        size_t j = 0;
        if (_left == _right) // Left, Right 가 같은 레이어인 경우, 중복 검사를 피하기 위함
            j = i;

        for (; j < rightSize; ++j)
        {
            // 충돌체가 존재하지 않거나, 자기 자신과 비교하는 경우
            CCollider* pRight = vecRight[j]->GetCollider();
            if (nullptr == pRight || vecLeft[i] == vecRight[j])
                continue;

            // 이전 프레임에 겹쳐 있었는지
            const ULONGLONG key  = MakeKey(pLeft->GetID(), pRight->GetID());
            const auto      iter = m_mapOverlap.find(key);
            const bool      bPrev = iter != m_mapOverlap.end();

            if (CollisionBtwCollider(pLeft, pRight))
            {
                if (bPrev)
                {
                    pLeft->OnOverlap(pRight);
                    pRight->OnOverlap(pLeft);
                }
                else
                {
                    m_mapOverlap.insert(make_pair(key, tOverlap{ pLeft, pRight }));
                    pLeft->BeginOverlap(pRight);
                    pRight->BeginOverlap(pLeft);
                }
            }
            else if (bPrev)
            {
                // 겹침이 끝났다
                m_mapOverlap.erase(iter);
                pLeft->EndOverlap(pRight);
                pRight->EndOverlap(pLeft);
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

    return true;
}

void CCollisionMgr::Reset()
{
    Clear();
    m_mapOverlap.clear();
}

void CCollisionMgr::ReleaseObject(CObj* _pObj)
{
    const CCollider* pCollider = _pObj->GetCollider();
    if (nullptr == pCollider)
        return;

    // 겹쳐 있던 쌍을 먼저 모두 떼어 낸 뒤 알린다 (알림 안에서 다른 오브젝트가 지워질 수도 있다)
    vector<tOverlap> vecEnded;
    for (auto iter = m_mapOverlap.begin(); iter != m_mapOverlap.end();)
    {
        if (iter->second.pLeft == pCollider || iter->second.pRight == pCollider)
        {
            vecEnded.push_back(iter->second);
            iter = m_mapOverlap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    for (const tOverlap& tPair : vecEnded)
    {
        tPair.pLeft->EndOverlap(tPair.pRight);
        tPair.pRight->EndOverlap(tPair.pLeft);
    }
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
