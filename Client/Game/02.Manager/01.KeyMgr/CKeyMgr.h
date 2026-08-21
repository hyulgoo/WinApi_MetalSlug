#pragma once

enum class KEY
{
	UP,		// VK_UP
	DOWN,
	LEFT,
	RIGHT,

	SPACE,
	ENTER,
	ESC,
	LALT,
	LCTRL,
	LSHIFT,
	TAB,

	LBTN,
	RBTN,


	Q,
	W,
	E,
	R,

	A,
	S,
	D,
	F,

	I,
	K,
	J,
	L,
	T,

	Z,
	X,
	C,
	V,

	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	END,
};

enum class KEY_STATE
{
	TAP,
	PRESSED,
	RELEASE,
	NONE,
};

struct tKeyInfo
{
	KEY			key;
	KEY_STATE	state;
	bool		bPrev;
};




class CKeyMgr
{
	SINGLE(CKeyMgr)

private:
	vector<tKeyInfo>	m_vecKey;
	Vec2				m_vMousePos;

public:
	void		init();
	void		tick();

public:
	KEY_STATE	GetKeyState(KEY _key) const { return m_vecKey[static_cast<UINT>(_key)].state; }
	Vec2		GetMousePos() { return m_vMousePos; }
};

