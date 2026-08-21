#include "pch.h"
#include "CLevel.h"
#include "CObj.h"
#include "CEngine.h"
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
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		for (UINT j = 0; j < m_arrLayer[i].size(); ++j)
			m_arrLayer[i][j]->tick();
	}
}

void CLevel::final_tick()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		for (UINT j = 0; j < m_arrLayer[i].size(); ++j)
			m_arrLayer[i][j]->final_tick();
	}
}

void CLevel::render(HDC _dc)
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		vector<CObj*>::iterator iter = m_arrLayer[i].begin();
		for (; iter != m_arrLayer[i].end();)
		{
			if ((*iter)->IsDead())
			{
				iter = m_arrLayer[i].erase(iter);
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
	for (size_t i = 0; i < m_arrLayer[(UINT)_Layer].size() + 1; ++i)
	{
		if (i = m_arrLayer[(UINT)_Layer].size())
			return nullptr;

		m_arrLayer[(UINT)_Layer][i] = _pObj;
		return _pObj;
	}
}

void CLevel::DeleteObject()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		for (UINT j = 0; j < m_arrLayer[i].size(); ++j)
			DEL(m_arrLayer[i][j]);

		m_arrLayer[i].clear();
	}
}

void CLevel::DeleteObject(LAYER _Layer)
{
	for (UINT i = 0; i < m_arrLayer[(UINT)_Layer].size(); ++i)
		delete(m_arrLayer[(UINT)_Layer][i]);

	m_arrLayer[(UINT)_Layer].clear();
}

 void CLevel::SetFocusedUI(CObj* _pUI)
 {
 	vector<CObj*>& vecUI = m_arrLayer[(UINT)LAYER::UI];
 
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

 void CLevel::SetRenderCollider()
 {
	 for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	 {
		 for (size_t j = 0; j < m_arrLayer[i].size(); ++j)
		 {
			 if (m_arrLayer[i][j]->GetCollider() == nullptr)
				 continue;
			 m_arrLayer[i][j]->GetCollider()->SetRender();
		 }
	 }
 }
