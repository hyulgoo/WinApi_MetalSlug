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

const CAnimData* CResMgr::LoadAnimData(const wstring& _strRelativePath)
{
    // "Enemy//a.anim"·"Enemy\\a.anim"처럼 다르게 적은 같은 파일을 하나로 본다
    wstring strKey;
    for (const wchar_t ch : _strRelativePath)
    {
        const wchar_t c = (L'/' == ch) ? L'\\' : static_cast<wchar_t>(towlower(ch));
        if (L'\\' == c && !strKey.empty() && L'\\' == strKey.back())
            continue;
        strKey += c;
    }

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
	pTexture = new CTexture;
	pTexture->Load(strFilePath);
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
