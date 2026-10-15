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

// 디버그 키(무적·충돌체 표시·순간 이동·좌표 표시 등)는 Debug 빌드에서만 동작한다
#ifdef _DEBUG
constexpr bool DEBUG_KEYS = true;
#else
constexpr bool DEBUG_KEYS = false;
#endif

// 스테이지 배경 텍스처 1px이 월드에서 차지하는 길이.
// 월드 좌표는 화면 픽셀 배율이다(카메라가 처음 자리에 있으면 월드 좌표 = 화면 좌표).
// 배경·픽셀 충돌 맵·레벨 파일(카메라 경로, 몬스터 배치)은 배경 텍스처 좌표다
constexpr float BG_MAGNI = 4.f;


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
};

// 몬스터 상태 (FSM, CAI). 몬스터마다 쓰는 상태만 등록한다
enum class MON_STATE
{
	IDLE,           // 플레이어를 기다린다
	TRACE,          // 쫓아간다
	TRACE_SLY,      // 천천히 다가간다 (병사)
	ATTACK,         // 공격 (좀비 토사물, 병사 수류탄)
	ATTACK_CQC,     // 근접 공격 (병사 칼)
	DEAD,           // 죽는다 (사망 이펙트를 남기고 사라진다)

	END,
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