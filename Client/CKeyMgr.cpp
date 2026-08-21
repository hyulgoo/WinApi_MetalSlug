#include "pch.h"
#include "CKeyMgr.h"
#include "CEngine.h"

int g_arrVK[static_cast<UINT>(KEY::END)]
=
{
	 VK_UP,
	 VK_DOWN,
	 VK_LEFT,
	 VK_RIGHT,

	 VK_SPACE,
	 VK_RETURN,
	 VK_ESCAPE,
	 VK_LMENU,
	 VK_LCONTROL,
	 VK_LSHIFT,
	 VK_TAB,

	 VK_LBUTTON,
	 VK_RBUTTON,

	 'Q',
	 'W',
	 'E',
	 'R',

	 'A',
	 'S',
	 'D',
	 'F',

	 'I',
	 'K',
	 'J',
	 'L',
	 'T',

	 'Z',
	 'X',
	 'C',
	 'V',

	 '0',
	 '1',
	 '2',
	 '3',
	 '4',
	 '5',
	 '6',
	 '7',
	 '8',

	 VK_F1,
	 VK_F2,
	 VK_F3,
	 VK_F4,
	 VK_F5,
	 VK_F6,
};


CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
}


void CKeyMgr::init()
{
	for (int i = 0; i < static_cast<int>(KEY::END); ++i)
	{
		m_vecKey.push_back(tKeyInfo{ static_cast<KEY>(i)  , KEY_STATE::NONE });
	}
}

void CKeyMgr::tick()
{
	if (GetFocus())
	{
		for (tKeyInfo& info : m_vecKey)
        {
			if (GetAsyncKeyState(g_arrVK[static_cast<UINT>(info.key)]) & 0x8000)
			{
				// 이전에는 눌려있지 않았다.
				if (false == info.bPrev)
				{
                    info.state = KEY_STATE::TAP;
                    info.bPrev = true;
				}
				else
				{
					// 지금도 눌려있고, 이전 프레임에서도 눌려있었다.
                    info.state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// 눌려있지 않다.
				if (false == info.bPrev)
				{
                    info.state = KEY_STATE::NONE;
				}
				else
				{
                    info.state = KEY_STATE::RELEASE;
                    info.bPrev = false;
				}
			}
		}
		POINT ptMousePos = {};
		GetCursorPos(&ptMousePos);
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
		m_vMousePos = ptMousePos;
	}
	// Window가 focus 상태가 아니다
	else
	{
		for (tKeyInfo& info : m_vecKey)
        {
			if (KEY_STATE::TAP == info.state || KEY_STATE::PRESSED == info.state)
                info.state = KEY_STATE::RELEASE;
			else if (KEY_STATE::RELEASE == info.state)
                info.state = KEY_STATE::NONE;
		}
	}
}