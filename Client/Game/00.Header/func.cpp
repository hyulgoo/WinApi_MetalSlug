#include "pch.h"
#include "Game\00.Header\func.h"

#include "Game\04.Object\CObj.h"
#include "Game\02.Manager\05.EventMgr\CEventMgr.h"

bool IsValid(CObj*& _pTarget)
{
    if (nullptr == _pTarget )
        return false;
    
    if(_pTarget->IsDead())
    {
        _pTarget = nullptr;
        return false;
    }

    return true;
}
    // 파일에 문자열을 저장
void SaveWString(const wstring& _str, FILE* _pFile)
{
    const size_t iLen = _str.length();
    // 저장할 주소, 저장할 크기, 저장할 개수, 파일스트림
    fwrite(&iLen, sizeof(size_t), 1, _pFile);
    fwrite(_str.c_str(), sizeof(wchar_t), iLen, _pFile);
}
    // 파일에 저장된 문자열을 읽어옴
void LoadWString(wstring& _str, FILE* _pFile)
{
    size_t iLen = 0;
    fread(&iLen, sizeof(size_t), 1, _pFile);

    wchar_t szBuff[256] = {};
    fread(szBuff, sizeof(wchar_t), iLen, _pFile);
    _str = szBuff;
}

void Instantiate(CObj* _pNewObj, const Vec2& _vPos, LAYER _eLayer)
{
    _pNewObj->SetPos(_vPos);
    tEvent _evn = {};
    _evn.eType = EVENT_TYPE::CREATE_OBJECT;
    _evn.wParam = (DWORD_PTR)_pNewObj;
    _evn.lParam = static_cast<DWORD_PTR>(_eLayer);

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
    _evn.wParam = static_cast<DWORD_PTR>(_eNextLevel);
    CEventMgr::GetInst()->AddEvent(_evn);
}
