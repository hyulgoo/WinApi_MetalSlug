#pragma once

#include <deque>

class CObj;
enum class KEY;

// 자동 테스트 모드
// 명령줄 --test <시나리오 파일> [--out <출력 폴더>] 로 켠다. 켜지면
//  - 창을 띄우지 않고, DT를 고정하고, 난수 시드를 고정한다
//  - 키·마우스 입력을 시나리오에서 읽는다
//  - 매 프레임 상태를 로그로 남기고, 시나리오의 기대값을 검사한 뒤 종료 코드로 결과를 알린다
// 같은 시나리오는 항상 같은 결과를 내므로 리팩토링 전후를 비교하는 데 쓴다.
// 시나리오 문법은 tests/README.md 에 있다.
class CTestMgr
{
    SINGLE(CTestMgr);

public:
    enum EXIT_CODE
    {
        EXIT_PASS         = 0,
        EXIT_EXPECT_FAIL  = 1,
        EXIT_SCRIPT_ERROR = 2,
        EXIT_CRASH        = 3,
        EXIT_CRT_ERROR    = 4,
    };

private:
    struct tKeyRange
    {
        int  iFrom;
        int  iTo;
        UINT iKey;
    };

    struct tMouse
    {
        int  iFrame;
        Vec2 vPos;
    };

    // 특정 프레임 끝에 실행하는 명령 (level, spawn, god, hmg, capture, expect)
    struct tCommand
    {
        int     iFrame;     // -1 이면 마지막 프레임 끝
        wstring strCmd;
        wstring strArg1;
        wstring strArg2;
        wstring strArg3;
        wstring strArg4;
        int     iLine;
    };

    bool                    m_bActive;
    bool                    m_bFinished;
    int                     m_iExitCode;

    wstring                 m_strScriptPath;
    wstring                 m_strName;
    wstring                 m_strOutDir;

    float                   m_fFixedDT;
    unsigned int            m_iSeed;
    int                     m_iMaxFrame;
    int                     m_iFrame;
    int                     m_iLogEvery;
    int                     m_iMonLogEvery;
    bool                    m_bNoSpawn;
    bool                    m_bAllowLeaks;
    bool                    m_bHeapCheck;
    bool                    m_bHasStartLevel;
    LEVEL_TYPE              m_eStartLevel;

    vector<tKeyRange>       m_vecKey;
    vector<tMouse>          m_vecMouse;
    vector<tCommand>        m_vecCommand;
    std::deque<wstring>     m_queDialogPath;

    // 집계
    map<wstring, int>       m_mapCreated;
    map<wstring, int>       m_mapDeleted;
    map<wstring, int>       m_mapLevelEnter;     // 레벨별 진입 횟수
    map<wstring, long long> m_mapStat;           // gdi.enter.<LEVEL>.<n> 등 시점 값
    int                     m_iErrorCount;
    int                     m_iExpectCount;
    int                     m_iExpectFail;

    FILE*                   m_pLog;

public:
    // 명령줄을 해석해 테스트 모드를 켠다. 테스트 모드가 아니면 false
    bool Init(const wchar_t* _szCmdLine);
    bool IsActive() const { return m_bActive; }
    bool IsFinished() const { return m_bFinished; }
    int  GetExitCode() const { return m_iExitCode; }
    int  GetFrame() const { return m_iFrame; }

    float      GetFixedDT() const { return m_fFixedDT; }
    bool       IsKeyDown(KEY _eKey) const;
    Vec2       GetMousePos() const;
    bool       IsSpawnDisabled() const { return m_bActive && m_bNoSpawn; }
    LEVEL_TYPE GetStartLevel(LEVEL_TYPE _eDefault) const { return m_bHasStartLevel ? m_eStartLevel : _eDefault; }
    bool       PopDialogPath(wstring& _strPath);

    // 엔진에서 부르는 알림
    void OnFrameEnd();
    void OnObjectCreated(CObj* _pObj, LAYER _eLayer);
    void OnObjectDeleted(CObj* _pObj);
    void OnLevelEnter(LEVEL_TYPE _eLevel);
    void OnError(const wchar_t* _szTitle, const wchar_t* _szMsg);

    // 로그 한 줄 (테스트 모드에서만 기록)
    void Log(const wchar_t* _szFormat, ...);

    // 크래시·CRT 오류 처리기가 쓰는 즉시 종료
    void Abort(int _iExitCode, const wchar_t* _szReason);

private:
    bool LoadScript();
    bool ParseLine(const wstring& _strLine, int _iLine);
    void RunCommand(const tCommand& _tCmd);
    void CheckExpect(const tCommand& _tCmd);
    bool GetSubject(const wstring& _strSubject, wstring& _strValue, double& _dValue, bool& _bNumber);
    void LogState();
    void LogMonsters();
    void Capture(const wstring& _strName) const;
    void Finish();

    wstring MakeOutPath(const wchar_t* _szExt) const;
};

// 레벨 이름 <-> 열거값
const wchar_t* LevelToStr(LEVEL_TYPE _eLevel);
bool           StrToLevel(const wchar_t* _szName, LEVEL_TYPE& _eLevel);
