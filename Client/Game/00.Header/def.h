#pragma once


#define SINGLE(type) public:\
					 static type* GetInst()\
					 {\
						 static type inst;\
						 return &inst;\
					 }\
					 private:\
						type();\
						~type();

#define DT CTimeMgr::GetInst()->GetDeltaTime()


#define IsTap(key) CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::TAP
#define IsPressed(key) CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::PRESSED
#define IsRelease(key) CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::RELEASE
#define MOUSE_POS CKeyMgr::GetInst()->GetMousePos()

#define DEL(p) if(nullptr != p) delete p;
#define PI 3.1415926535f

#define CLONE(type) virtual type* Clone() { return new type(*this); }
#define CLONE_DEACTIVATE(type) virtual type* Clone() { assert(nullptr); return nullptr; }

#define TILE_SIZE 64


enum class LEVEL_TYPE
{
	GAMEOVER,
	START,
	STAGE,
	ANIMEDITOR,

	END,
};

enum class PEN_TYPE
{
	RED,
	GREEN,
	BLUE,
	WHITE,
	NONE,

	END,
};


enum class LAYER
{
	BACKGROUND_BACK,
	BACKGROUND,
	DEFAULT,
	MONSTER,
	PLAYER,
	PLAYER_PROJECTILE,
	PLAYER_FIREPROJECTILE,
	CQC,
	CQCAREA,
	MONSTER_PROJECTILE,
	ITEM,
	EFFECT,

	BACKGROUND_FRONT,
	UI,
	EDITUI,
	EDITANIM,
	END = 16	
};


enum class EVENT_TYPE
{
	CREATE_OBJECT,	 //	 wParam : Object Adress   lParam : Layer Type
	DELETE_OBJECT, 	 //  wParam : Object Adress
	LEVEL_CHANGE,	 //  wParam : LEVEL_TYPE(Next Level Type)
	CHANGE_AI_STATE, //  wParam : AI Component Adress, lParam : Next State Name
};

enum class CAMERA_EFFECT
{
	FADE_IN,
	FADE_OUT,
	MOVE,
	NONE,
};

enum class SETPRINT
{
	UPPER,
	NORMAL,
	LOWER
};

enum class DIR
{
	RIGHT,
	UP,
	LEFT,
	DOWN,
	NONE,
};

enum class MONSTER
{
	SOLDIER,
	ZB_GIRL,
	ZB_BOY,
	ZB_MAN,
	ZB_OLD,
	ZB_RSH,
	END,
};