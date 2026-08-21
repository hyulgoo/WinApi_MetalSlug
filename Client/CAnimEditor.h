#pragma once
#include "CLevel.h"

class CTexture;
class CEditPlayer;
class CEditorPanel;
class CAnimator;
class CEditAnim;


enum class PRINTMODE
{
    UPPER,
    NORMAL,
    LOWER
};

class CAnimEditor :
    public CLevel
{
private:
    HMENU               m_hMenu;
    CObj*               m_pEditImage;
    CEditAnim*          m_pEidtAnim;
    vector<tAnimFrm>    m_vecPrevUpperFrm;
    vector<tAnimFrm>    m_vecWorkUpperFrm;
    vector<tAnimFrm>    m_vecPrevLowerFrm;
    vector<tAnimFrm>    m_vecWorkLowerFrm;
    vector<tAnimFrm>    m_vecPrevNormalFrm;
    vector<tAnimFrm>    m_vecWorkNormalFrm;
    tAnimFrm            m_tCurFrm;

    bool                m_bIsStarted;
    bool                m_bIsPressed;

    POINT               m_ptRealStart;
    POINT               m_ptRealEnd;
    POINT               m_ptStart;
    POINT               m_ptEnd;
    Vec2                m_vecDragRectSize;
    Vec2                m_vResolution;

    float               m_fmagni;
    int                 m_iCreatFrmCount;
    CEditorPanel*       m_pEditPanel;
    float               m_fKeyPressDelay;
    PRINTMODE           m_tPrintMode;
    bool                m_bAnimator1;
    bool                m_bAnimator2;
    bool                m_bPressMode;
    bool                m_bOffsetPress;

public:
    CEditPlayer*        m_pEditPlayer;
    CTexture*           m_pAtlas;
    CAnimator*          m_pAnimator;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Exit() override;
    virtual void Enter() override;

public:
    void SaveUpperAnimation();
    void SaveLowerAnimation();
    void SaveNormalAnimation();
    void LoadUpperAnimation();
    void LoadLowerAnimation();
    void LoadNormalAnimation();
    void PlayLeft();
    void PlayRight();

    void PrintDragArea();
    void PrintCreateAnimation();
    void PrintMouseInfo(HDC _dc);
    void PrintEditAnimInfo(HDC _dc, CAnimator* pAnimator);
    void PrintMousePos(HDC _dc);
    void PrintInfo(HDC _dc);

    void CreateUI();
    void CreateAnimation();

    void TimeStop();

    void SetCurFrm(CAnimator* _pAnimator);
    void FirstFrm(CAnimator* _pAnimator);
    void PrevFrm(CAnimator* _pAnimator);
    void NextFrm(CAnimator* _pAnimator);

public:
    CAnimEditor();
    ~CAnimEditor();
};

