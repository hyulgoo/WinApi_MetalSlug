#pragma once
#include "CEntity.h"


class CObj;

class CLevel :
    public CEntity
{
private:
    vector<CObj*>   m_arrLayer[(UINT)LAYER::END];

public:
    virtual void init() = 0;
    virtual void tick();
    virtual void render(HDC _dc);
    virtual void final_tick();

    virtual void Enter() = 0;
    virtual void Exit() = 0;

public:
    void AddObject(CObj* _pObj, LAYER _Layer)     {  m_arrLayer[(UINT)_Layer].push_back(_pObj); }
    CObj* GetLayerObject(CObj* _pObj, LAYER _Layer);
    const vector<CObj*>& GetLayer(LAYER _layer) { return m_arrLayer[(UINT)_layer]; }
    vector<CObj*> GetALLLayer() {return m_arrLayer[(UINT)LAYER::END]; }
    void DeleteObject();
    void DeleteObject(LAYER _Layer);

    void SetFocusedUI(CObj* _pUI);

    void SetRenderCollider();
  
public:
    CLONE_DEACTIVATE(CLevel);

public:
    CLevel();
    virtual ~CLevel();
};

