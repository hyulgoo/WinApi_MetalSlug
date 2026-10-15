#include "pch.h"
#include "Game\02.Manager\06.PathMgr\CPathMgr.h"

#include "Game\01.Engine\CEngine.h"

CPathMgr::CPathMgr()
	: m_szContentPath{}
{
}

CPathMgr::~CPathMgr()
{
}

namespace
{
	// 전체 경로에서 뒤에서부터 '\\'를 찾아 0으로 바꿔서 경로를 자른다.
	void CutLastPath(wchar_t* _szPath)
	{
		for (int i = static_cast<int>(wcslen(_szPath)) - 1; i >= 0; --i)
		{
			if (L'\\' == _szPath[i])
			{
				_szPath[i] = 0;
				break;
			}
		}
	}

	bool IsDirectory(const wchar_t* _szPath)
	{
		const DWORD dwAttr = GetFileAttributes(_szPath);
		return INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	}
}

void CPathMgr::init()
{
	// 실행 파일(OutputFile\bin 또는 bin_d) 기준으로 OutputFile\bin\content 를 찾는다.
	// VS 디버거(F5)는 작업 디렉터리가 프로젝트 폴더라서 현재 디렉터리만으로는 경로가 어긋난다.
	GetModuleFileName(nullptr, m_szContentPath, 256);
	CutLastPath(m_szContentPath); // 실행 파일 이름
	CutLastPath(m_szContentPath); // bin 또는 bin_d
	wcscat_s(m_szContentPath, L"\\bin\\content\\");

	// 실행 파일이 다른 곳에 있으면 기존처럼 현재 디렉터리 기준으로 찾는다.
	if (!IsDirectory(m_szContentPath))
	{
		GetCurrentDirectory(256, m_szContentPath);
		CutLastPath(m_szContentPath);
		wcscat_s(m_szContentPath, L"\\bin\\content\\");
	}
}

