#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <commdlg.h>

#include <crtdbg.h>
#include <assert.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <queue>

using std::queue;
using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::string;
using std::wstring;


#include "Game\00.Header\def.h"
#include "Game\00.Header\struct.h"
#include "Game\00.Header\func.h"

#pragma comment (lib, "Msimg32.lib")
