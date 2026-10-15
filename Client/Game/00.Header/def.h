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


// 식 전체를 괄호로 감싼다 (예전에는 !IsTap(k) 나 IsTap(k) == b 처럼 쓰면 연산자 우선순위가 바뀌었다)
#define IsTap(key) (CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::TAP)
#define IsPressed(key) (CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::PRESSED)
#define IsRelease(key) (CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::RELEASE)
#define MOUSE_POS CKeyMgr::GetInst()->GetMousePos()

#define PI 3.1415926535f

#define CLONE(type) virtual type* Clone() { return new type(*this); }
#define CLONE_DEACTIVATE(type) virtual type* Clone() { assert(nullptr); return nullptr; }

// 디버그 키(무적·충돌체 표시·순간 이동·좌표 표시 등)는 Debug 빌드에서만 동작한다
#ifdef _DEBUG
constexpr bool DEBUG_KEYS = true;
#else
constexpr bool DEBUG_KEYS = false;
#endif

// 게임 수치는 content\data\*.ini 에 있다 (CDataMgr). 아래 두 배율은 game.ini [scale]이고 실행할 때 한 번 읽는다

// 스테이지 배경 텍스처 1px이 월드에서 차지하는 길이.
// 월드 좌표는 화면 픽셀 배율이다(카메라가 처음 자리에 있으면 월드 좌표 = 화면 좌표).
// 배경·픽셀 충돌 맵·레벨 파일(카메라 경로, 몬스터 배치)은 배경 텍스처 좌표다
#define BG_MAGNI CDataMgr::GetInst()->GetBgMagni()

// 애니메이션 시트 1px을 화면에 그리는 배율 (오프셋도 이 배율로 커진다)
#define ANIM_MAGNI CDataMgr::GetInst()->GetAnimMagni()


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
	OBSTACLE,	// 위에 올라설 수 있는 장애물 (보스가 떨어뜨린 비석). 픽셀 충돌이 발판으로 찾는다
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
	EDITANIM,
	END,	// 개수. 충돌 매니저가 레이어 쌍을 WORD 비트로 담으므로 16을 넘으면 안 된다 (CCollisionMgr.h)
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
	APPEAR,         // 땅에서 솟아오른다 (보스)
	WAIT,           // 다음 공격까지 기다린다 (보스)

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
	BOSS,       // 모세의 십계 (CBoss)
	END,
};