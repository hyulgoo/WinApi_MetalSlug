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

	int iLen = (int)wcslen(m_szContentPath);

	// 현재 경로에서 뒤에서부터 '\\'를 찾아 0으로 바꿔 끝으로 인식하게 함.
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

void CPathMgr::render()
{
	// 제목을 표시할 윈도우창, 문구
	SetWindowText(CEngine::GetInst()->GetMainWnd(), m_szContentPath);
}
