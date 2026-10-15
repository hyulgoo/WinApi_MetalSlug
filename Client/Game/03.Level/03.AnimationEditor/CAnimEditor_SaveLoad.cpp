#include "pch.h"
#include "Game\03.Level\03.AnimationEditor\CAnimEditor.h"

#include "Game\01.Engine\CEngine.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"
#include "Game\02.Manager\11.DataMgr\CDataMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

#include "resource.h"

#include <algorithm>
#include <cmath>
#include <cwctype>

namespace
{
    // 경로의 마지막 이름 (파일 이름)
    wstring FileName(const wstring& _strPath)
    {
        const size_t iSlash = _strPath.find_last_of(L"\\/");
        return wstring::npos == iSlash ? _strPath : _strPath.substr(iSlash + 1);
    }

    bool HasSpace(const wstring& _str)
    {
        return std::any_of(_str.begin(), _str.end(), [](const wchar_t _ch) { return 0 != iswspace(_ch); });
    }

    // 이름으로 미리보기 방식을 짐작한다 (게임은 상체·하체 애니메이션을 "_UPPER"·"_LOWER"로 부른다)
    CAnimEditor::PREVIEW GuessPreview(const wstring& _strName)
    {
        if (wstring::npos != _strName.find(L"UPPER"))
            return CAnimEditor::PREVIEW::UPPER;
        if (wstring::npos != _strName.find(L"LOWER"))
            return CAnimEditor::PREVIEW::LOWER;
        return CAnimEditor::PREVIEW::NORMAL;
    }

    // content 폴더 기준 경로. content 밖이면 빈 문자열
    wstring ToContentRelative(const wstring& _strFullPath)
    {
        wchar_t szFull[MAX_PATH]    = {};
        wchar_t szContent[MAX_PATH] = {};
        GetFullPathName(_strFullPath.c_str(), MAX_PATH, szFull, nullptr);
        GetFullPathName(CPathMgr::GetInst()->GetContentPath(), MAX_PATH, szContent, nullptr);
        const size_t iLen = wcslen(szContent);
        if (0 == iLen || 0 != _wcsnicmp(szFull, szContent, iLen))
            return L"";
        return szFull + iLen;
    }

    // "격자로 프레임 추가" 대화상자 (WM_INITDIALOG의 lParam으로 받는다)
    struct tGridDialog
    {
        CAnimEditor::tGridInput* pInput = nullptr;
        Vec2                     vSheetSize;
    };

    INT_PTR CALLBACK GridDlgProc(const HWND _hDlg, const UINT _iMsg, const WPARAM _wParam, const LPARAM _lParam)
    {
        constexpr int arrID[] = { IDC_GRID_LEFT, IDC_GRID_TOP, IDC_GRID_WIDTH, IDC_GRID_HEIGHT, IDC_GRID_COUNT, IDC_GRID_DURATION };

        switch (_iMsg)
        {
        case WM_INITDIALOG:
        {
            SetWindowLongPtr(_hDlg, GWLP_USERDATA, _lParam);
            const CAnimEditor::tGridInput& tIn = *reinterpret_cast<const tGridDialog*>(_lParam)->pInput;
            const int arrValue[] = { tIn.iLeft, tIn.iTop, tIn.iWidth, tIn.iHeight, tIn.iCount, tIn.iDuration };
            for (int i = 0; i < 6; ++i)
                SetDlgItemInt(_hDlg, arrID[i], static_cast<UINT>(arrValue[i]), FALSE);
            return TRUE;
        }
        case WM_COMMAND:
            if (IDOK == LOWORD(_wParam))
            {
                tGridDialog*            pDlg = reinterpret_cast<tGridDialog*>(GetWindowLongPtr(_hDlg, GWLP_USERDATA));
                CAnimEditor::tGridInput tIn;
                int* arrOut[] = { &tIn.iLeft, &tIn.iTop, &tIn.iWidth, &tIn.iHeight, &tIn.iCount, &tIn.iDuration };
                for (int i = 0; i < 6; ++i)
                {
                    BOOL bOk  = FALSE;
                    *arrOut[i] = static_cast<int>(GetDlgItemInt(_hDlg, arrID[i], &bOk, FALSE));
                    if (!bOk)
                    {
                        MessageBox(_hDlg, L"모든 칸에 0 이상의 정수를 넣으세요", L"격자로 프레임 추가", MB_OK);
                        return TRUE;
                    }
                }

                // 틀리면 대화상자를 닫지 않고 고치게 한다 (예전에는 검사 조건이 반대였고, 확인을 눌러도 닫히지 않았다)
                const wstring strError = CAnimEditor::CheckGridInput(tIn, pDlg->vSheetSize);
                if (!strError.empty())
                {
                    MessageBox(_hDlg, strError.c_str(), L"격자로 프레임 추가", MB_OK);
                    return TRUE;
                }
                *pDlg->pInput = tIn;
                EndDialog(_hDlg, IDOK);
                return TRUE;
            }
            if (IDCANCEL == LOWORD(_wParam))
            {
                EndDialog(_hDlg, IDCANCEL);
                return TRUE;
            }
            break;
        default:
            break;
        }
        return FALSE;
    }
}

// ---------------------------------------------------------------- 파일

bool CAnimEditor::ShowAnimFileDialog(const bool _bSave, wstring& _strPath)
{
    // 전체 경로 버퍼와 파일 이름 버퍼를 따로 둔다 (예전에는 같은 버퍼라 경로가 파일 이름으로 덮였고,
    // 대화상자가 바꾼 현재 디렉터리 기준 상대 경로로 열었다, MS-R40)
    wchar_t szFile[MAX_PATH]  = {};
    wchar_t szTitle[MAX_PATH] = {};
    if (_bSave && !m_tDoc.strName.empty())
        wcscpy_s(szFile, m_tDoc.strName.c_str());

    // 처음 폴더: 마지막으로 쓴 파일의 폴더, 없으면 content\animation
    wstring strDir = wstring(CPathMgr::GetInst()->GetContentPath()) + L"animation\\";
    if (!m_strFilePath.empty())
        strDir = m_strFilePath.substr(0, m_strFilePath.find_last_of(L"\\/") + 1);

    OPENFILENAME ofn    = {};
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = CEngine::GetInst()->GetMainWnd();
    ofn.lpstrFilter     = L"Animation (*.anim)\0*.anim\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = szFile;
    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrFileTitle  = szTitle;
    ofn.nMaxFileTitle   = MAX_PATH;
    ofn.lpstrInitialDir = strDir.c_str();
    ofn.lpstrDefExt     = L"anim";   // 확장자를 안 쓰면 붙인다 (예전에는 확장자 없는 Player/asd 같은 파일이 생겼다)
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | (_bSave ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);

    const bool bOk = ShowFileDialog(ofn, _bSave);
    m_bWaitRelease = true;
    if (!bOk)
        return false;
    _strPath = szFile;
    return true;
}

void CAnimEditor::Save()
{
    // 프레임이 없으면 파일을 고르기 전에 멈춘다 (예전에는 고른 파일을 먼저 비운 뒤 애니메이션이 없어 크래시했다, MS-R22)
    if (m_tDoc.vecFrm.empty())
    {
        ReportError(L"애니메이션 저장", L"저장할 프레임이 없습니다. 시트에서 드래그해 프레임을 만드세요.");
        return;
    }

    wstring strPath;
    if (!ShowAnimFileDialog(true, strPath))
        return;

    // 애니메이션 이름 = 파일 이름 = 게임에서 Play하는 이름. 아틀라스는 편집 중인 시트 그대로
    // (예전에는 미리보기 쪽에 고정한 아틀라스를 기록해, 다른 시트의 애니메이션을 불러와 저장하면 아틀라스가 바뀌었다, MS-R21)
    tAnimFile tOut = m_tDoc;
    tOut.strName   = FileName(strPath);
    wstring strError;
    if (!CAnimData::WriteFile(strPath, tOut, strError))
    {
        ReportError(L"애니메이션 저장 실패", strError.c_str());
        return;
    }

    m_tDoc.strName = tOut.strName;
    m_strFilePath  = strPath;
    m_iSavedSerial = m_iSerial;

    // 게임이 이미 읽어 둔 파일이면 새 내용으로 바꾼다 (다시 실행하지 않아도 게임에 보인다)
    CResMgr::GetInst()->ReloadAnimData(strPath);
}

void CAnimEditor::Load()
{
    if (!ConfirmDiscard())
        return;

    // 대화상자를 먼저 띄우고, 고른 파일을 다 읽은 뒤에야 문서를 바꾼다
    // (예전에는 대화상자를 띄우기 전에 작업 버퍼를 비워, 취소해도 프레임이 사라졌다, MS-R22)
    wstring strPath;
    if (!ShowAnimFileDialog(false, strPath))
        return;

    tAnimFile tIn;
    wstring   strError;
    if (!CAnimData::ReadFile(strPath, tIn, strError))
    {
        ReportError(L"애니메이션 불러오기 실패", strError.c_str());
        return;
    }
    if (tIn.strAtlasKey.empty() || tIn.strAtlasPath.empty())
    {
        const wstring strMsg = L"아틀라스 정보가 없는 파일입니다\n" + strPath;
        ReportError(L"애니메이션 불러오기 실패", strMsg.c_str());
        return;
    }
    if (tIn.strName.empty())
        tIn.strName = FileName(strPath);

    // 시트는 파일이 쓰는 아틀라스로 바꾼다
    AddSheet(tIn.strAtlasKey, tIn.strAtlasPath);
    CTexture* pSheet = CResMgr::GetInst()->LoadTexture(tIn.strAtlasKey, tIn.strAtlasPath);
    m_ePreview       = GuessPreview(tIn.strName);
    SetDoc(tIn, pSheet);
    m_strFilePath = strPath;
    ScrollToFrame(0, false);
}

void CAnimEditor::LoadReference()
{
    wstring strPath;
    if (!ShowAnimFileDialog(false, strPath))
        return;

    tAnimFile tIn;
    wstring   strError;
    if (!CAnimData::ReadFile(strPath, tIn, strError))
    {
        ReportError(L"참고 애니메이션 불러오기 실패", strError.c_str());
        return;
    }
    if (tIn.strAtlasKey.empty() || tIn.strAtlasPath.empty() || tIn.vecFrm.empty())
    {
        const wstring strMsg = L"아틀라스나 프레임이 없는 파일입니다\n" + strPath;
        ReportError(L"참고 애니메이션 불러오기 실패", strMsg.c_str());
        return;
    }
    if (tIn.strName.empty())
        tIn.strName = FileName(strPath);

    m_tRef        = tIn;
    m_eRefPreview = GuessPreview(tIn.strName);
    SyncPreview(true);
}

void CAnimEditor::ClearReference()
{
    m_tRef = {};
    SyncPreview(true);
}

// ---------------------------------------------------------------- 시트

void CAnimEditor::ScanSheets()
{
    // content\animation 아래 .anim 파일들이 쓰는 아틀라스를 모은다 (키 순서)
    m_vecSheet.clear();
    vector<wstring> vecDir = { wstring(CPathMgr::GetInst()->GetContentPath()) + L"animation\\" };
    while (!vecDir.empty())
    {
        const wstring strDir = vecDir.back();
        vecDir.pop_back();

        WIN32_FIND_DATA tFind = {};
        const HANDLE    hFind = FindFirstFile((strDir + L"*").c_str(), &tFind);
        if (INVALID_HANDLE_VALUE == hFind)
            continue;
        do
        {
            const wstring strName = tFind.cFileName;
            if (L"." == strName || L".." == strName)
                continue;
            if (tFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                vecDir.push_back(strDir + strName + L"\\");
                continue;
            }
            if (strName.size() < 5 || 0 != _wcsicmp(strName.c_str() + strName.size() - 5, L".anim"))
                continue;

            tAnimFile tFile;
            wstring   strError;
            if (CAnimData::ReadFile(strDir + strName, tFile, strError) && !tFile.strAtlasKey.empty() && !tFile.strAtlasPath.empty())
                AddSheet(tFile.strAtlasKey, tFile.strAtlasPath);
        } while (FindNextFile(hFind, &tFind));
        FindClose(hFind);
    }

    std::sort(m_vecSheet.begin(), m_vecSheet.end(), [](const tSheet& _a, const tSheet& _b) { return _a.strKey < _b.strKey; });
}

void CAnimEditor::AddSheet(const wstring& _strKey, const wstring& _strPath)
{
    if (FindSheet(_strKey) < 0)
        m_vecSheet.push_back(tSheet{ _strKey, _strPath });
}

int CAnimEditor::FindSheet(const wstring& _strKey) const
{
    for (int i = 0; i < static_cast<int>(m_vecSheet.size()); ++i)
    {
        if (m_vecSheet[i].strKey == _strKey)
            return i;
    }
    return -1;
}

void CAnimEditor::ChangeSheet(const int _iDir)
{
    if (m_vecSheet.empty() || !ConfirmDiscard())
        return;

    const int iCount = static_cast<int>(m_vecSheet.size());
    const int iCur   = FindSheet(m_tDoc.strAtlasKey);
    SelectSheet(iCur < 0 ? 0 : ((iCur + _iDir) % iCount + iCount) % iCount);
}

void CAnimEditor::OpenSheetFile()
{
    if (!ConfirmDiscard())
        return;

    wchar_t       szFile[MAX_PATH]  = {};
    wchar_t       szTitle[MAX_PATH] = {};
    const wstring strDir            = wstring(CPathMgr::GetInst()->GetContentPath()) + L"texture\\";

    OPENFILENAME ofn    = {};
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = CEngine::GetInst()->GetMainWnd();
    ofn.lpstrFilter     = L"Bitmap (*.bmp)\0*.bmp\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = szFile;
    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrFileTitle  = szTitle;
    ofn.nMaxFileTitle   = MAX_PATH;
    ofn.lpstrInitialDir = strDir.c_str();
    ofn.lpstrDefExt     = L"bmp";
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    const bool bOk      = ShowFileDialog(ofn, false);
    m_bWaitRelease      = true;
    if (!bOk)
        return;

    // 아틀라스 경로는 content 폴더 기준으로 저장한다
    const wstring strRelative = ToContentRelative(szFile);
    if (strRelative.empty() || HasSpace(strRelative))
    {
        const wstring strMsg = L"content 폴더 안의, 경로에 공백이 없는 BMP만 시트로 쓸 수 있습니다\n" + wstring(szFile);
        ReportError(L"시트 열기", strMsg.c_str());
        return;
    }

    // 게임이 이미 쓰는 그림이면 그 키를 쓴다 (같은 파일을 다른 키로 두 번 읽지 않게).
    // 처음 보는 그림이면 파일 이름을 키로, 다른 그림이 쓰는 키와 겹치면 뒤에 _를 붙인다
    wstring strKey;
    for (const tSheet& tSheetInfo : m_vecSheet)
    {
        if (0 == _wcsicmp(tSheetInfo.strPath.c_str(), strRelative.c_str()))
        {
            strKey = tSheetInfo.strKey;
            break;
        }
    }
    if (strKey.empty())
    {
        const wstring strFile = FileName(strRelative);
        strKey                = strFile.substr(0, strFile.find_last_of(L'.'));
        while (true)
        {
            const int iSheet = FindSheet(strKey);
            CTexture* pTex   = CResMgr::GetInst()->FindTexture(strKey);
            const bool bUsed = (iSheet >= 0 && 0 != _wcsicmp(m_vecSheet[iSheet].strPath.c_str(), strRelative.c_str()))
                            || (nullptr != pTex && 0 != _wcsicmp(pTex->GetRelativePath().c_str(), strRelative.c_str()));
            if (!bUsed)
                break;
            strKey += L"_";
        }
    }

    AddSheet(strKey, strRelative);
    SelectSheet(FindSheet(strKey));
}

// ---------------------------------------------------------------- 격자로 프레임 추가

wstring CAnimEditor::CheckGridInput(const tGridInput& _tIn, const Vec2& _vSheetSize)
{
    if (_tIn.iLeft < 0 || _tIn.iTop < 0)
        return L"왼쪽 위 좌표는 0 이상이어야 합니다";
    if (_tIn.iWidth < 1 || _tIn.iHeight < 1)
        return L"프레임 크기는 1 이상이어야 합니다";
    const int iMaxCount = CDataMgr::GetInst()->GetSection(L"editor", L"edit").GetInt(L"max_grid_count");
    if (_tIn.iCount < 1 || _tIn.iCount > iMaxCount)
        return L"프레임 수는 1~" + std::to_wstring(iMaxCount) + L"이어야 합니다";
    if (_tIn.iDuration < 1)
        return L"재생 시간은 1(0.01초) 이상이어야 합니다";

    const long long llRight  = static_cast<long long>(_tIn.iLeft) + static_cast<long long>(_tIn.iWidth) * _tIn.iCount;
    const long long llBottom = static_cast<long long>(_tIn.iTop) + _tIn.iHeight;
    if (llRight > static_cast<long long>(_vSheetSize.x) || llBottom > static_cast<long long>(_vSheetSize.y))
        return L"프레임이 시트 밖으로 나갑니다";
    return L"";
}

void CAnimEditor::OpenGridDialog()
{
    if (nullptr == m_pSheet)
    {
        ReportError(L"격자로 프레임 추가", L"시트가 없습니다");
        return;
    }
    const Vec2 vSheetSize(static_cast<float>(m_pSheet->Width()), static_cast<float>(m_pSheet->Height()));

    // 처음 값은 고르고 있는 프레임 (드래그로 한 칸을 잡고 개수만 넣으면 된다). 없으면 data\editor.ini [grid]
    tGridInput tIn = m_tData.tGrid;
    Vec2       vOffset;
    if (const tAnimFrm* pCur = CurFrm())
    {
        tIn.iLeft     = static_cast<int>(pCur->vLeftTop.x);
        tIn.iTop      = static_cast<int>(pCur->vLeftTop.y);
        tIn.iWidth    = static_cast<int>(pCur->vSize.x);
        tIn.iHeight   = static_cast<int>(pCur->vSize.y);
        tIn.iDuration = max(1, static_cast<int>(lroundf(pCur->fDuration * 100.f)));
        vOffset       = pCur->vOffset;
    }

    if (CTestMgr::GetInst()->IsActive())
    {
        // 자동 테스트: 대화상자 대신 시나리오의 dialog 값 "왼쪽,위,너비,높이,개수,시간(1/100초)"
        wstring strAnswer;
        if (!CTestMgr::GetInst()->PopDialogPath(strAnswer))
        {
            CTestMgr::GetInst()->Log(L"DIALOG %d -> cancel", CTestMgr::GetInst()->GetFrame());
            return;
        }
        if (6 != swscanf_s(strAnswer.c_str(), L"%d,%d,%d,%d,%d,%d", &tIn.iLeft, &tIn.iTop, &tIn.iWidth, &tIn.iHeight, &tIn.iCount, &tIn.iDuration))
        {
            ReportError(L"격자로 프레임 추가", L"dialog 값은 왼쪽,위,너비,높이,개수,시간 입니다");
            return;
        }
        CTestMgr::GetInst()->Log(L"DIALOG %d -> %s", CTestMgr::GetInst()->GetFrame(), strAnswer.c_str());

        const wstring strError = CheckGridInput(tIn, vSheetSize);
        if (!strError.empty())
        {
            ReportError(L"격자로 프레임 추가", strError.c_str());
            return;
        }
    }
    else
    {
        tGridDialog   tDlg    = { &tIn, vSheetSize };
        const INT_PTR iResult = DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_GRIDFRAMES), CEngine::GetInst()->GetMainWnd()
                                             , GridDlgProc, reinterpret_cast<LPARAM>(&tDlg));
        m_bWaitRelease = true;
        if (IDOK != iResult)
            return;
    }

    // 문서에 더한다 (예전에는 임시 에디터에 만들어 바로 저장해서 화면에는 보이지 않았다, MS-R25)
    PushUndo();
    const int iFirst = static_cast<int>(m_tDoc.vecFrm.size());
    for (int i = 0; i < tIn.iCount; ++i)
    {
        tAnimFrm frm  = {};
        frm.vLeftTop  = Vec2(static_cast<float>(tIn.iLeft + i * tIn.iWidth), static_cast<float>(tIn.iTop));
        frm.vSize     = Vec2(static_cast<float>(tIn.iWidth), static_cast<float>(tIn.iHeight));
        frm.vOffset   = vOffset;
        frm.fDuration = static_cast<float>(tIn.iDuration) / 100.f;
        m_tDoc.vecFrm.push_back(frm);
    }
    m_iCurFrm = iFirst;
    OnDocChanged();
}
