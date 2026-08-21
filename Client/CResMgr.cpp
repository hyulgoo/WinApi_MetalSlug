#include "pch.h"
#include "CResMgr.h"

#include "CPathMgr.h"
#include "CTexture.h"

CResMgr::CResMgr()
{

}
CResMgr::~CResMgr()
{
	map<wstring, CTexture*>::iterator iter = m_mapTex.begin();

	for (; iter != m_mapTex.end(); ++iter)
	{
		delete iter->second;
	}
}

CTexture* CResMgr::FindTexture(const wstring& _strKey)
{
	map<wstring, CTexture*>::iterator iter = m_mapTex.find(_strKey);
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
		return (CTexture*)pTexture;

	// PathMgr를 이용해서 텍스쳐의 최종 경로를 만든다.
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	// 텍스쳐 생성 및 로딩
	pTexture = new CTexture;
	pTexture->Load(strFilePath);
	pTexture->SetKey(_strKey);
	pTexture->SetRelativePath(_strRelativePath);

	// Map에 저장
	m_mapTex.insert(make_pair(_strKey, (CTexture*)pTexture));

	return (CTexture*)pTexture;
}

CTexture* CResMgr::CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _Height)
{
	CTexture* pTexture = FindTexture(_strKey);

	if (nullptr != pTexture)
		return (CTexture*)pTexture;

	// 텍스쳐 생성 및 로딩
	pTexture = new CTexture;
	pTexture->Create(_iWidth, _Height);
	pTexture->SetKey(_strKey);

	// Map에 저장
	m_mapTex.insert(make_pair(_strKey, (CTexture*)pTexture));

	return (CTexture*)pTexture;
	return nullptr;
}
