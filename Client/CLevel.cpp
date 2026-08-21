#include "pch.h"
#include "CLevel.h"
#include "CObj.h"
#include "CCollider.h"

CLevel::CLevel()
{
}

CLevel::~CLevel()
{
    DeleteObject();
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
    for (auto& layer : m_arrLayer)
    {
        vector<CObj*>::iterator iter = layer.begin();
        while (iter != layer.end())
        {
            if ((*iter)->IsDead())
            {
                iter = layer.erase(iter);
            }
            else
            {
                (*iter)->render(_dc);
                ++iter;
            }
        }
    }
}

CObj* CLevel::GetLayerObject(CObj* _pObj, LAYER _Layer)
{
    const size_t size = m_arrLayer[static_cast<UINT>(_Layer)].size();
    for (size_t i = 0; i <= size; ++i)
    {
        if (i == size)
            return nullptr;

        m_arrLayer[static_cast<UINT>(_Layer)][i] = _pObj;
        return _pObj;
    }
    
    return nullptr;
}

void CLevel::DeleteObject()
{
    for (auto& layer : m_arrLayer)
    {
        for (const auto& obj : layer)
            DEL(obj)
        layer.clear();
    }
}

void CLevel::DeleteObject(LAYER _Layer)
{
    for (const auto& obj : m_arrLayer[static_cast<UINT>(_Layer)])
        delete obj;

    m_arrLayer[static_cast<UINT>(_Layer)].clear();
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
