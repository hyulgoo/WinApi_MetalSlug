#include "pch.h"
#include "Game\03.Level\CLevel.h"
#include "Game\04.Object\CObj.h"
#include "Game\05.Component\Collider\CCollider.h"

#include <algorithm>

CLevel::CLevel()
{
}

CLevel::~CLevel()
{
    DeleteAllObjects();
}

void CLevel::tick()
{
    for (auto& layer : m_arrLayer)
        for (const auto& obj : layer)
            obj->tick();
}

void CLevel::final_tick()
{
    for (auto& layer : m_arrLayer)
        for (const auto& obj : layer)
            obj->final_tick();
}

void CLevel::render(const HDC _dc)
{
    // 삭제 표시된 오브젝트는 CEventMgr가 이미 레이어에서 뺐다
    for (auto& layer : m_arrLayer)
        for (const auto& obj : layer)
            obj->render(_dc);
}

void CLevel::AddObject(CObj* _pObj, const LAYER _Layer)
{
    _pObj->m_eLevelLayer = _Layer;
    m_arrLayer[static_cast<UINT>(_Layer)].push_back(_pObj);
}

void CLevel::RemoveObject(CObj* _pObj)
{
    vector<CObj*>& vecLayer = m_arrLayer[static_cast<UINT>(_pObj->m_eLevelLayer)];
    const auto     iter     = std::find(vecLayer.begin(), vecLayer.end(), _pObj);
    if (iter != vecLayer.end())
        vecLayer.erase(iter);
}

void CLevel::DeleteAllObjects()
{
    for (auto& layer : m_arrLayer)
    {
        for (const auto& obj : layer)
            DEL(obj)
        layer.clear();
    }
}

void CLevel::ClearLayer(const LAYER _Layer)
{
    for (CObj* obj : m_arrLayer[static_cast<UINT>(_Layer)])
    {
        if (!obj->IsDead())
            obj->SetDead();
    }
}

void CLevel::SetFocusedUI(CObj* _pUI)
{
    vector<CObj*>& vecUI = m_arrLayer[static_cast<UINT>(LAYER::UI)];

    if (vecUI.back() == _pUI)
        return;

    vector<CObj*>::iterator iter = vecUI.begin();
    for (; iter != vecUI.end(); ++iter)
    {
        if ((*iter) == _pUI)
        {
            vecUI.erase(iter);
            vecUI.push_back(_pUI);
            return;
        }
    }

    assert(nullptr);
}

void CLevel::SetRenderCollider() const
{
    for (const auto& objs : m_arrLayer)
    {
        for (auto obj : objs)
        {
            if (obj->GetCollider() == nullptr)
                continue;
            obj->GetCollider()->SetRender();
        }
    }
}
