#pragma once
#include "CEntity.h"
class CRes :
    public CEntity
{
private:
    // 리소스의 키값과 로딩상대경로
    wstring     m_strKey;
    wstring     m_strRelativePath;
public:
    const wstring& GetKey() { return m_strKey; }
    const wstring& GetRelativePath() { return m_strRelativePath; }

private:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }

protected:
    virtual int Load(const wstring& _strFilePath) = 0;
    CLONE_DEACTIVATE(CRes);

public:
    CRes();
    virtual ~CRes();

    // Texture 및 sound의 부모인 Res를 공개하여 부모타입으로 자식개체의 
    // 가상함수 private에 접근할 수 있음.
    friend class CResMgr;
};

