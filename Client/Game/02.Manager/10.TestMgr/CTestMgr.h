#pragma once

#include <deque>
#include <set>

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
    bool                    m_bDebugOnly;        // Debug 빌드에서만 돌린다 (debugonly)
    bool                    m_bSkipped;          // Release라서 건너뛰었다
    bool                    m_bHasStartLevel;
    LEVEL_TYPE              m_eStartLevel;

    vector<tKeyRange>       m_vecKey;
    vector<tMouse>          m_vecMouse;
    vector<tCommand>        m_vecCommand;
    std::deque<wstring>     m_queDialogPath;

    map<int, float>         m_mapSpikeDT;        // 프레임별 DT 덮어쓰기 (spike)
    vector<wstring>         m_vecWatch;          // 위치 범위를 추적할 클래스 (watch)
    vector<wstring>         m_vecWatchAnim;      // 애니메이션 전환을 셀 클래스 (watchanim)
    vector<wstring>         m_vecTrace;          // 매 프레임 위치·애니메이션 프레임을 로그로 남길 클래스 (trace)

    // 집계
    map<wstring, int>       m_mapCreated;
    map<wstring, int>       m_mapDeleted;
    map<wstring, int>       m_mapFirstCreated;   // 클래스별 첫 생성 프레임
    map<wstring, int>       m_mapFirstDeleted;   // 클래스별 첫 삭제 프레임
    map<wstring, int>       m_mapLifetime;       // 클래스별로 처음 삭제된 오브젝트가 산 프레임 수
    map<wstring, int>       m_mapDeletedFrm;     // 클래스별로 처음 삭제된 오브젝트가 지워질 때의 애니메이션 프레임
    map<wstring, int>       m_mapFirstAnim;      // watchanim: "<클래스>.<애니메이션>[@<프레임 번호>]"를 처음 보인 프레임
    map<UINT, int>          m_mapCreateFrame;    // 살아 있는 오브젝트 ID별 생성 프레임
    map<wstring, int>       m_mapLevelEnter;     // 레벨별 진입 횟수
    map<wstring, long long> m_mapStat;           // gdi.enter.<LEVEL>.<n> 등 시점 값
    map<wstring, long long> m_mapCounter;        // 게임 코드가 AddCount로 세는 값
    map<wstring, Vec2>      m_mapMin;            // watch 클래스의 위치 최솟값
    map<wstring, Vec2>      m_mapMax;            // watch 클래스의 위치 최댓값
    map<wstring, Vec2>      m_mapWMin;           // watch 클래스의 월드 위치 최솟값
    map<wstring, Vec2>      m_mapWMax;           // watch 클래스의 월드 위치 최댓값
    map<wstring, double>    m_mapMaxColLag;      // watch 클래스의 충돌체 지연 거리 최댓값
    map<UINT, wstring>      m_mapLastAnim;       // watchanim: 오브젝트 ID별 직전 애니메이션
    map<wstring, int>       m_mapAnimCount;      // watchanim: "<클래스>.<애니메이션>" 시작 횟수
    int                     m_iErrorCount;
    double                  m_dTickMsSum   = 0.0;   // AddFrameTiming 합계 (perf.tick·perf.render 평균용)
    double                  m_dRenderMsSum = 0.0;
    int                     m_iTimedFrames = 0;
    std::set<wstring>       m_setAnimFile;       // 읽은 .anim 파일 (경로 구분자를 맞춘 소문자)
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

    float      GetFixedDT() const;
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

    // 게임 코드가 세는 값 (예: 픽셀 충돌 맵 블릿 횟수). 시나리오에서 count.<이름> 으로 읽는다
    void AddCount(const wchar_t* _szName, long long _llAdd = 1);

    // 엔진이 프레임마다 tick·render에 걸린 시간(ms)을 알린다. 처음 몇 프레임(레벨 진입·리소스 로드)은 빼고 평균을 낸다
    void AddFrameTiming(double _dTickMs, double _dRenderMs);

    // .anim 파일을 디스크에서 읽어 해석할 때마다 알린다 (anim.parse 횟수와 서로 다른 파일 수 anim.files)
    void OnAnimParse(const wstring& _strPath);

    // 로그 한 줄 (테스트 모드에서만 기록)
    void Log(const wchar_t* _szFormat, ...);

    // 크래시·CRT 오류 처리기가 쓰는 즉시 종료
    void Abort(int _iExitCode, const wchar_t* _szReason);

private:
    bool LoadScript();
    bool ParseLine(const wstring& _strLine, int _iLine);
    bool ParseFrame(const wstring& _str, int& _iOut) const;   // 프레임 번호 또는 "2.5s"(초)
    void RunCommand(const tCommand& _tCmd);
    void CheckExpect(const tCommand& _tCmd);
    bool GetSubject(const wstring& _strSubject, wstring& _strValue, double& _dValue, bool& _bNumber);
    bool GetObjectField(CObj* _pObj, const wstring& _strField, wstring& _strValue, double& _dValue, bool& _bNumber);
    void LogState();
    void LogMonsters();
    void TrackWatches();
    void Capture(const wstring& _strName) const;
    void Finish();

    wstring MakeOutPath(const wchar_t* _szExt) const;
};

// 레벨 이름 <-> 열거값
const wchar_t* LevelToStr(LEVEL_TYPE _eLevel);
bool           StrToLevel(const wchar_t* _szName, LEVEL_TYPE& _eLevel);
