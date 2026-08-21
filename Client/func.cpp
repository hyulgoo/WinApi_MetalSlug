#include "pch.h"
#include "func.h"

#include "CObj.h"
#include "CEventMgr.h"

bool IsValid(CObj*& _pTarget)
{
    if (nullptr == _pTarget )
        return false;
    else if(_pTarget->IsDead())
    {
        _pTarget = nullptr;
        return false;
    }    

    return true;
}
    // 파일에 지정한 문자를 입력
void SaveWString(const wstring& _str, FILE* _pFile)
{
    size_t iLen = _str.length();
    // 변수의 주소, 변수의 크기, 변수의 개수, 파일스트림
    fwrite(&iLen, sizeof(size_t), 1, _pFile);
    fwrite(_str.c_str(), sizeof(wchar_t), iLen, _pFile);
}
    // 파일에 지정한 문자를 읽어옴
void LoadWString(wstring& _str, FILE* _pFile)
{
    size_t iLen = 0;
    fread(&iLen, sizeof(size_t), 1, _pFile);

    wchar_t szBuff[256] = {};
    fread(szBuff, sizeof(wchar_t), iLen, _pFile);
    _str = szBuff;
}

void Instantiate(CObj* _pNewObj, Vec2 _vPos, LAYER _eLayer)
{
    _pNewObj->SetPos(_vPos);
    tEvent _evn = {};
    _evn.eType = EVENT_TYPE::CREATE_OBJECT;
    _evn.wParam = (DWORD_PTR)_pNewObj;
    _evn.lParam = (DWORD_PTR)_eLayer;

    CEventMgr::GetInst()->AddEvent(_evn);
}

void Saturate(float& _float)
{
    if (1.f < _float)
        _float = 1.f;
    if (0.f > _float)
        _float = 0.f;
}

void ChangeLevel(LEVEL_TYPE _eNextLevel)
{
    tEvent _evn = {};
    _evn.eType = EVENT_TYPE::LEVEL_CHANGE;
    _evn.wParam = (DWORD_PTR)_eNextLevel;
    CEventMgr::GetInst()->AddEvent(_evn);
}
