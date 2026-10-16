#include "pch.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"
#include "Game\06.Resource\Anim\CAnimData.h"
#include "Game\02.Manager\10.TestMgr\CTestMgr.h"

CResMgr::CResMgr()
{

}
CResMgr::~CResMgr()
{
    for (auto& iter : m_mapAnim)
        delete iter.second;
    for (auto& iter : m_mapTex)
        delete iter.second;
}

namespace
{
    // "Enemy//a.anim"·"Enemy\\a.anim"처럼 다르게 적은 같은 파일을 하나로 본다
    wstring MakeAnimKey(const wstring& _strRelativePath)
    {
        wstring strKey;
        for (const wchar_t ch : _strRelativePath)
        {
            const wchar_t c = (L'/' == ch) ? L'\\' : static_cast<wchar_t>(towlower(ch));
            if (L'\\' == c && !strKey.empty() && L'\\' == strKey.back())
                continue;
            strKey += c;
        }
        return strKey;
    }
}

void CResMgr::ReloadAnimData(const wstring& _strFullPath)
{
    wchar_t szSaved[MAX_PATH] = {};
    GetFullPathName(_strFullPath.c_str(), MAX_PATH, szSaved, nullptr);

    // 읽어 둔 데이터마다 실제 파일 경로를 구해 비교한다 ("Enemy//a.anim"·"..\x.anim"처럼 적은 것도 같은 파일이면 맞다)
    const wstring strRoot = wstring(CPathMgr::GetInst()->GetContentPath()) + L"animation\\";
    for (const auto& iter : m_mapAnim)
    {
        CAnimData* pData           = iter.second;
        wchar_t    szFull[MAX_PATH] = {};
        GetFullPathName((strRoot + pData->GetRelativePath()).c_str(), MAX_PATH, szFull, nullptr);
        if (0 != _wcsicmp(szFull, szSaved))
            continue;

        // 같은 CAnimData에 다시 읽는다 (이 데이터를 보는 CAnimation들이 새 프레임을 본다)
        CTestMgr::GetInst()->OnAnimParse(pData->GetRelativePath());
        pData->m_strName.clear();
        pData->m_pAtlas = nullptr;
        pData->m_vecFrm.clear();
        pData->Load(szFull);
        if (pData->m_strName.empty())
        {
            const wstring& strRelative = pData->GetRelativePath();
            pData->m_strName           = strRelative.substr(strRelative.find_last_of(L"/\\") + 1);
        }
    }
}

const CAnimData* CResMgr::LoadAnimData(const wstring& _strRelativePath)
{
    const wstring strKey = MakeAnimKey(_strRelativePath);

    const auto iter = m_mapAnim.find(strKey);
    if (iter != m_mapAnim.end())
        return iter->second;

    CTestMgr::GetInst()->OnAnimParse(_strRelativePath);

    CAnimData* pData = new CAnimData;
    pData->SetKey(strKey);
    pData->SetRelativePath(_strRelativePath);
    pData->Load(wstring(CPathMgr::GetInst()->GetContentPath()) + L"animation\\" + _strRelativePath);

    // 읽지 못했으면 파일 이름을 애니메이션 이름으로 둔다 (예전에는 이름 ""으로 등록됐다)
    if (pData->m_strName.empty())
    {
        const size_t iSep = _strRelativePath.find_last_of(L"/\\");
        pData->m_strName  = (wstring::npos == iSep) ? _strRelativePath : _strRelativePath.substr(iSep + 1);
    }

    m_mapAnim.insert(make_pair(strKey, pData));
    return pData;
}

CTexture* CResMgr::FindTexture(const wstring& _strKey)
{
	const map<wstring, CTexture*>::iterator iter = m_mapTex.find(_strKey);
	if (iter == m_mapTex.end())
	{
		return nullptr;
	}
	return iter->second;
}

CTexture* CResMgr::LoadTexture(const wstring& _strKey, const wstring& _strRelativePath)
{
	CRes* pTexture = FindTexture(_strKey);

	if (nullptr != pTexture)
		return dynamic_cast<CTexture*>(pTexture);

	// PathMgr를 이용해서 텍스처의 절대 경로를 만든다.
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	// 텍스처 생성 후 로드
	CTexture* pNewTexture = new CTexture;
	if (FAILED(pNewTexture->Load(strFilePath)))
	{
		// 대신 1x1 마젠타(투명색) 텍스처를 같은 키로 둔다. 그려도 보이지 않고, 같은 키를 다시 찾아도 오류를 또 남기지 않는다
		delete pNewTexture;
		pNewTexture = new CTexture;
		pNewTexture->Create(1, 1);
		SetPixel(pNewTexture->GetDC(), 0, 0, RGB(255, 0, 255));
	}
	pTexture = pNewTexture;
	pTexture->SetKey(_strKey);
	pTexture->SetRelativePath(_strRelativePath);

	// Map에 등록
	m_mapTex.insert(make_pair(_strKey, dynamic_cast<CTexture*>(pTexture)));

	return dynamic_cast<CTexture*>(pTexture);
}

CTexture* CResMgr::CreateTexture(const wstring& _strKey, const UINT _iWidth, const UINT _Height)
{
	CTexture* pTexture = FindTexture(_strKey);

	if (nullptr != pTexture)
		return pTexture;

	// 텍스처 생성 후 로드
	pTexture = new CTexture;
	pTexture->Create(_iWidth, _Height);
	pTexture->SetKey(_strKey);

	// Map에 등록
	m_mapTex.insert(make_pair(_strKey, static_cast<CTexture*>(pTexture)));

	return pTexture;
}

CTexture* CResMgr::GetTintedTexture(CTexture* _pSrc, const COLORREF _tColor)
{
	if (nullptr == _pSrc)
		return nullptr;

	// 키: 원래 키 + 색 (색을 바꾸면 따로 만든다)
	wchar_t szColor[16] = {};
	swprintf_s(szColor, L"#tint%06X", static_cast<unsigned int>(_tColor));
	const wstring strKey = _pSrc->GetKey() + szColor;
	if (CTexture* pTinted = FindTexture(strKey))
		return pTinted;

	CTexture* pTinted = CreateTexture(strKey, _pSrc->Width(), _pSrc->Height());
	BitBlt(pTinted->GetDC(), 0, 0, static_cast<int>(_pSrc->Width()), static_cast<int>(_pSrc->Height()), _pSrc->GetDC(), 0, 0, SRCCOPY);
	pTinted->Tint(_tColor);
	return pTinted;
}
