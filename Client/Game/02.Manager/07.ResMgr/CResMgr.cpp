#include "pch.h"
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

#include "Game\02.Manager\06.PathMgr\CPathMgr.h"
#include "Game\06.Resource\Texture\CTexture.h"

CResMgr::CResMgr()
{

}
CResMgr::~CResMgr()
{
    for (auto& iter : m_mapTex)
        delete iter.second;
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
