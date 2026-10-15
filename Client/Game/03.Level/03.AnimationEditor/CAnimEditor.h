#pragma once
#include "Game\03.Level\CLevel.h"
#include "Game\06.Resource\Anim\CAnimData.h"

class CTexture;
class CEditPlayer;
class CEditorPanel;
enum class KEY;

// 애니메이션 에디터 (타이틀에서 F1)
// 편집 중인 애니메이션 하나(m_tDoc: 이름·아틀라스·프레임)가 단일 원본이다. 시트 위 프레임 사각형, 미리보기, 패널 정보,
// 저장 파일이 모두 이것을 그대로 보여 준다. 파일은 엔진과 같은 코드(CAnimData::ReadFile/WriteFile)로 읽고 쓴다.
// (예전에는 상체·하체·전신 작업 버퍼 세 벌, 미리보기 애니메이터 두 개, 코드에 고정한 시트 두 곳이 따로 놀았다, MS-R39)
class CAnimEditor :
    public CLevel
{
public:
    // 미리보기를 게임의 어느 출력 방식으로 그릴지 (SETPRINT와 같다. 파일에는 저장하지 않는다)
    enum class PREVIEW
    {
        NORMAL,
        UPPER,
        LOWER,
    };

    // "격자로 프레임 추가" 입력 (시트 픽셀, 재생 시간은 1/100초). 처음 값은 data\editor.ini [grid]
    struct tGridInput
    {
        int iLeft     = 0;
        int iTop      = 0;
        int iWidth    = 0;
        int iHeight   = 0;
        int iCount    = 0;
        int iDuration = 0;
    };

private:
    // 고를 수 있는 시트 (.anim 파일들이 쓰는 아틀라스 + 직접 연 BMP)
    struct tSheet
    {
        wstring strKey;    // 아틀라스 텍스처 키 (파일에 그대로 저장된다)
        wstring strPath;   // content 폴더 기준 경로
    };

    struct tUndo
    {
        vector<tAnimFrm> vecFrm;
        int              iCurFrm;
        int              iSerial;
    };

    // 수치 (data\editor.ini). 들어올 때마다 읽는다
    struct tData
    {
        float      fSheetMagni      = 0.f;   // [view] 들어올 때 시트 배율
        int        iPanelWidthDiv   = 0;     // 패널 폭 = 화면 너비 / 이 값
        float      fPreviewYRatio   = 0.f;
        float      fSheetKeep       = 0.f;
        float      fScrollSpeed     = 0.f;
        float      fScrollFast      = 0.f;
        float      fRepeatDelay     = 0.f;   // [input]
        float      fRepeatInterval  = 0.f;
        float      fDefaultDuration = 0.f;   // [edit]
        size_t     iMaxUndo         = 0;
        tGridInput tGrid;                    // [grid]
        Vec2       vButtonSize;              // [panel]
        int        iButtonsPerRow   = 0;
        float      fButtonGap       = 0.f;
        float      fMargin          = 0.f;
        int        iInfoTop         = 0;
        int        iLineHeight      = 0;
        COLORREF   tInfoColor       = 0;
        COLORREF   tSheetTextColor  = 0;
        Vec2       vFrameNumberOffset;
        Vec2       vDragTextOffset;
        Vec2       vMouseTextPos;            // y가 음수면 화면 아래에서
    };
    tData         m_tData;

    HMENU         m_hMenu      = nullptr;
    Vec2          m_vResolution;
    float         m_fPanelLeft = 0.f;      // 이 x부터 오른쪽은 패널 (시트 입력을 받지 않는다)
    CEditorPanel* m_pPanel     = nullptr;
    CEditPlayer*  m_pPreview   = nullptr;

    // 편집 중인 애니메이션
    tAnimFile     m_tDoc;
    CTexture*     m_pSheet       = nullptr;  // m_tDoc의 아틀라스
    int           m_iCurFrm      = 0;
    vector<tUndo> m_vecUndo;
    int           m_iSerial      = 0;        // 고칠 때마다 새 번호 (되돌리면 그때 번호로 돌아간다)
    int           m_iLastSerial  = 0;        // 지금까지 나눠 준 가장 큰 번호
    int           m_iSavedSerial = 0;        // 저장하거나 불러온 때의 번호. 다르면 저장하지 않은 변경이 있다
    wstring       m_strFilePath;             // 마지막으로 저장하거나 불러온 파일 (대화상자의 처음 폴더)

    // 시트 보기
    vector<tSheet> m_vecSheet;
    Vec2           m_vSheetPos;              // 시트 왼쪽 위의 화면 위치
    float          m_fSheetMagni = 0.f;      // 들어올 때 [view] sheet_magni, 1~4 키로 바꾼다

    // 마우스
    bool m_bDrag        = false;
    Vec2 m_vDragStart;                       // 시트 픽셀
    Vec2 m_vDragEnd;
    bool m_bPan         = false;
    Vec2 m_vPanMouse;
    Vec2 m_vPanSheet;
    bool m_bWaitRelease = false;             // 대화상자를 닫은 뒤 눌려 있던 왼쪽 버튼은 뗄 때까지 무시한다

    // 키
    map<KEY, float> m_mapRepeat;             // 누르고 있는 키의 반복 시간
    bool            m_bOffsetAll = false;    // F2: 오프셋을 모든 프레임에 더한다
    bool            m_bHelp      = true;     // F1: 조작법 표시

    // 미리보기
    PREVIEW   m_ePreview     = PREVIEW::NORMAL;
    bool      m_bPreviewLeft = false;
    bool      m_bPause       = false;        // SPACE: 미리보기만 멈춘다 (예전에는 전역 시간 정지였다)
    tAnimFile m_tRef;                        // 참고 애니메이션 (상체를 고칠 때 하체를 함께 보는 등. 읽기만 한다)
    PREVIEW   m_eRefPreview  = PREVIEW::NORMAL;

public:
    virtual void tick() override;
    virtual void render(HDC _dc) override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    // 패널 버튼·메뉴·단축키가 부르는 명령
    void NewDoc();
    void Load();
    void Save();
    void ChangeSheet(int _iDir);
    void OpenSheetFile();
    void OpenGridDialog();
    void LoadReference();
    void ClearReference();
    void DeleteFrame();
    void Undo();
    void GoTitle();

    // 자동 테스트용
    const tAnimFile& GetDoc() const { return m_tDoc; }
    int              GetCurFrm() const { return m_iCurFrm; }
    bool             IsDirty() const { return m_iSerial != m_iSavedSerial; }
    size_t           GetUndoCount() const { return m_vecUndo.size(); }
    PREVIEW          GetPreview() const { return m_ePreview; }
    bool             IsPaused() const { return m_bPause; }
    Vec2             GetSheetPos() const { return m_vSheetPos; }
    float            GetSheetMagni() const { return m_fSheetMagni; }
    const tAnimFile& GetReference() const { return m_tRef; }

    // 입력이 맞으면 빈 문자열, 틀리면 이유 (대화상자와 자동 테스트가 같이 쓴다)
    static wstring CheckGridInput(const tGridInput& _tIn, const Vec2& _vSheetSize);

private:
    void      LoadData();   // data\editor.ini

    // 문서
    void      SetDoc(const tAnimFile& _tDoc, CTexture* _pSheet);
    void      SelectSheet(int _iSheet);
    bool      ConfirmDiscard() const;
    void      PushUndo();
    void      OnDocChanged();
    tAnimFrm* CurFrm();
    void      SelectFrame(int _iFrm);
    void      SelectFrameAt(const Vec2& _vPixel);
    void      AddFrame(const Vec2& _vLeftTop, const Vec2& _vSize);
    void      MoveFrame(int _iDir);

    // 입력
    void UpdateCommandKeys();
    void UpdateSheetView();
    void UpdateMouse();
    void UpdateEditKeys();
    int  KeyRepeat(KEY _eKey);
    Vec2 SheetOrigin() const;
    Vec2 ToSheetPixel(const Vec2& _vScreen) const;
    bool IsOnSheetArea(const Vec2& _vScreen) const;    // 편집 영역(패널 밖)
    bool IsOnSheetImage(const Vec2& _vScreen) const;   // 시트 그림 위
    void SetSheetMagni(float _fMagni);
    void ClampSheetPos();
    void ScrollToFrame(int _iFrm, bool _bOnlyIfHidden);

    // 시트 목록·파일
    void ScanSheets();
    void AddSheet(const wstring& _strKey, const wstring& _strPath);
    int  FindSheet(const wstring& _strKey) const;
    bool ShowAnimFileDialog(bool _bSave, wstring& _strPath);

    // 미리보기·그리기
    // 문서를 미리보기에 옮긴다. _bReference면 참고 애니메이션(m_tRef)도 다시 만든다
    void SyncPreview(bool _bReference = false);
    void CreateUI();
    void RenderSheet(HDC _dc) const;
    void RenderFrames(HDC _dc) const;
    void RenderInfo(HDC _dc) const;

public:
    CAnimEditor();
    virtual ~CAnimEditor() override;
};
