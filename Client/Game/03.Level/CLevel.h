#pragma once
#include "Game\Entity\CEntity.h"

class CObj;
class CTexture;

class CLevel :
    public CEntity
{
private:
    vector<CObj*>   m_arrLayer[static_cast<UINT>(LAYER::END)];
    bool            m_arrCameraLayer[static_cast<UINT>(LAYER::END)] = {};   // 월드 좌표라 카메라를 따라 그리는 레이어
    CTexture*       m_pCollisionMap = nullptr;  // 픽셀 충돌이 조회하는 맵 (배경 텍스처 좌표). 없으면 픽셀 충돌을 하지 않는다

public:
    // 레벨 수명: 생성자는 영구 리소스만, Enter는 오브젝트 생성과 상태 초기화, Exit는 정리
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
    void                 DeleteAllObjects();             // 레벨을 나갈 때 모든 오브젝트를 바로 지운다 (프레임 경계에서만)
    void                 ClearLayer(LAYER _Layer);       // 레이어의 오브젝트에 삭제를 요청한다 (프레임 끝에 지워진다)

    void SetFocusedUI(CObj* _pUI);

    void SetCameraLayer(LAYER _eLayer) { m_arrCameraLayer[static_cast<UINT>(_eLayer)] = true; }
    void            SetCollisionMap(CTexture* _pMap) { m_pCollisionMap = _pMap; }
    const CTexture* GetCollisionMap() const { return m_pCollisionMap; }
    bool IsCameraLayer(LAYER _eLayer) const { return m_arrCameraLayer[static_cast<UINT>(_eLayer)]; }

    void SetRenderCollider() const;
  
public:
    CLONE_DEACTIVATE(CLevel)

public:
    CLevel();
    virtual ~CLevel();
};

