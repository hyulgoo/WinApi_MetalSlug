#pragma once
#include "Game\Entity\CEntity.h"

class CObj;

class CLevel :
    public CEntity
{
private:
    vector<CObj*>   m_arrLayer[static_cast<UINT>(LAYER::END)];

public:
    virtual void init() = 0;
    virtual void tick();
    virtual void render(HDC _dc);
    virtual void final_tick();

    virtual void Enter() = 0;
    virtual void Exit() = 0;

public:
    void                 AddObject(CObj* _pObj, LAYER _Layer);
    void                 RemoveObject(CObj* _pObj);     // 레이어에서 빼기만 한다 (CEventMgr가 삭제 요청을 처리할 때)
    CObj*                GetLayerObject(CObj* _pObj, LAYER _Layer);
    const vector<CObj*>& GetLayer(LAYER _layer) { return m_arrLayer[static_cast<UINT>(_layer)]; }
    vector<CObj*>        GetALLLayer() {return m_arrLayer[static_cast<UINT>(LAYER::END)]; }
    void                 DeleteObject();                 // 레벨을 나갈 때 모든 오브젝트를 바로 지운다 (프레임 경계에서만)
    void                 ClearLayer(LAYER _Layer);       // 레이어의 오브젝트에 삭제를 요청한다 (프레임 끝에 지워진다)

    void SetFocusedUI(CObj* _pUI);

    void SetRenderCollider() const;
  
public:
    CLONE_DEACTIVATE(CLevel)

public:
    CLevel();
    virtual ~CLevel();
};

