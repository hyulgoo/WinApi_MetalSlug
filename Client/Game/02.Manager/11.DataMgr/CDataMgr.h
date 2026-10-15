#pragma once

#include <set>

// 게임 수치 데이터 파일(content\data\<파일>.ini)의 [구역] 하나.
// 값은 물을 때 해석한다. 없거나 형식이 틀린 값은 키마다 한 번만 알리고 0을 돌려준다
class CDataSection
{
    friend class CDataMgr;

private:
    wstring               m_strFile;          // 오류 메시지용 (player)
    wstring               m_strName;          // 구역 이름
    map<wstring, wstring> m_mapValue;         // 키 -> 값 (앞뒤 공백·주석을 뺀 글자 그대로)
    bool                  m_bMissing = false; // 파일이나 구역이 없다 (이미 한 번 알렸으니 키마다 다시 알리지 않는다)

    mutable std::set<wstring> m_setReported;  // 이미 알린 키

public:
    float    GetFloat(const wchar_t* _szKey) const;
    double   GetDouble(const wchar_t* _szKey) const;
    int      GetInt(const wchar_t* _szKey) const;
    Vec2     GetVec2(const wchar_t* _szKey) const;     // "x, y"
    COLORREF GetColor(const wchar_t* _szKey) const;    // "r, g, b" (0~255)

private:
    const wstring* Find(const wchar_t* _szKey) const;
    void           ReportBad(const wchar_t* _szKey, const wchar_t* _szWhy) const;
};

// 게임 수치 데이터 파일을 읽어 둔다. 파일은 처음 물을 때 읽고, 레벨을 바꿀 때 다시 읽는다
// (게임을 끄지 않고 파일을 고친 뒤 레벨에 다시 들어가면 바뀐 값으로 만든다).
// 창 크기·프레임·배율(game.ini [window]·[time]·[scale])은 실행할 때 한 번만 쓴다
class CDataMgr
{
    SINGLE(CDataMgr)
private:
    // 자동 테스트가 바꿔 쓰는 값 하나
    struct tOverride
    {
        wstring strFile;
        wstring strSection;
        wstring strKey;
        wstring strValue;
    };

    map<wstring, map<wstring, CDataSection>> m_mapFile;         // 파일 -> 구역 -> 값
    std::set<wstring>                        m_setMissingFile;  // 읽지 못한 파일 (구역마다 다시 알리지 않는다)
    vector<tOverride>                        m_vecOverride;

    float m_fBgMagni   = 0.f;
    float m_fAnimMagni = 0.f;

public:
    void init();        // 실행할 때 한 번 쓰는 배율을 읽는다 (CPathMgr 다음)
    void Reload();      // 읽어 둔 값을 버린다. 다음에 물을 때 파일을 다시 읽는다

    // 구역을 돌려준다. 돌려받은 참조는 그 자리에서 값을 읽는 데만 쓴다 (Reload가 지운다)
    const CDataSection& GetSection(const wchar_t* _szFile, const wchar_t* _szSection);

    float GetBgMagni() const { return m_fBgMagni; }
    float GetAnimMagni() const { return m_fAnimMagni; }

    // 자동 테스트: 파일의 값 대신 쓴다 (다시 읽어도 남는다). 파일에 없는 키면 그 파일을 읽을 때 알린다
    void SetOverride(const wstring& _strFile, const wstring& _strSection, const wstring& _strKey, const wstring& _strValue);

private:
    map<wstring, CDataSection>& LoadFile(const wstring& _strFile);
};
