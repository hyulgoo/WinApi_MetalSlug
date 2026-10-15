#include "pch.h"
#include "Game\00.Header\func.h"

#include "Game\04.Object\CObj.h"
#include "Game\02.Manager\05.EventMgr\CEventMgr.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

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

bool AskYesNo(const wchar_t* _szTitle, const wchar_t* _szMsg)
{
    if (CTestMgr::GetInst()->IsActive())
    {
        CTestMgr::GetInst()->Log(L"ASK %d [%s] %s -> yes", CTestMgr::GetInst()->GetFrame(), _szTitle, _szMsg);
        return true;
    }
    return IDYES == MessageBox(nullptr, _szMsg, _szTitle, MB_YESNO);
}

bool ShowFileDialog(OPENFILENAME& _tOfn, const bool _bSave)
{
    if (!CTestMgr::GetInst()->IsActive())
        return _bSave ? FALSE != GetSaveFileName(&_tOfn) : FALSE != GetOpenFileName(&_tOfn);

    // 자동 테스트: 대화상자가 돌려주는 것과 같게 채운다
    // (lpstrFile 에 전체 경로, lpstrFileTitle 에 파일 이름, OFN_NOCHANGEDIR 이 없으면 현재 디렉터리를 파일 폴더로,
    //  저장 대화상자에서 확장자를 안 쓰면 lpstrDefExt 를 붙인다)
    wstring strPath;
    if (!CTestMgr::GetInst()->PopDialogPath(strPath))
    {
        CTestMgr::GetInst()->Log(L"DIALOG %d -> cancel", CTestMgr::GetInst()->GetFrame());
        return false;
    }
    if (strPath.size() < 2 || L':' != strPath[1])
        strPath = wstring(CPathMgr::GetInst()->GetContentPath()) + strPath;

    size_t iSlash = strPath.find_last_of(L"\\/");
    if (_bSave && nullptr != _tOfn.lpstrDefExt && wstring::npos == strPath.find(L'.', wstring::npos == iSlash ? 0 : iSlash + 1))
        strPath += wstring(L".") + _tOfn.lpstrDefExt;
    iSlash = strPath.find_last_of(L"\\/");
    wcscpy_s(_tOfn.lpstrFile, _tOfn.nMaxFile, strPath.c_str());
    if (nullptr != _tOfn.lpstrFileTitle && wstring::npos != iSlash)
        wcscpy_s(_tOfn.lpstrFileTitle, _tOfn.nMaxFileTitle, strPath.substr(iSlash + 1).c_str());
    if (!(_tOfn.Flags & OFN_NOCHANGEDIR) && wstring::npos != iSlash)
        SetCurrentDirectory(strPath.substr(0, iSlash).c_str());

    CTestMgr::GetInst()->Log(L"DIALOG %d -> %s", CTestMgr::GetInst()->GetFrame(), strPath.c_str());
    return true;
}
