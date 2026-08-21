#include "pch.h"
#include "CPathMgr.h"

#include "CEngine.h"

CPathMgr::CPathMgr()
	: m_szContentPath{}
{
}

CPathMgr::~CPathMgr()
{
}

void CPathMgr::init()
{
	GetCurrentDirectory(256, m_szContentPath);

	const int iLen = static_cast<int>(wcslen(m_szContentPath));

	// 전체 경로에서 뒤에서부터 '\\'를 찾아 0으로 바꿔서 경로를 자른다.
	for (int i = iLen - 1; i >= 0; --i)
	{
		if (L'\\' == m_szContentPath[i])
		{
			m_szContentPath[i] = 0;
			break;
		}
	}

	wcscat_s(m_szContentPath, L"\\bin\\content\\");

	render();
}

void CPathMgr::render() const
{
	// 타이틀바에 경로 표시
	SetWindowText(CEngine::GetInst()->GetMainWnd(), m_szContentPath);
}
