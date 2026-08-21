#pragma once

struct Vec2
{
	float x;
	float y;


public:
	void Normalize() // 정규화
	{
		float fDist = sqrtf(x * x + y * y);
		x /= fDist;
		y /= fDist;
	}

	bool IsZero()
	{
		return x == 0.f && y == 0.f;
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	Vec2 operator +(Vec2 _vOther)
	{
		return Vec2(x + _vOther.x, y + _vOther.y);
	}

	void operator += (Vec2 _vOther)
	{
		x += _vOther.x;
		y += _vOther.y;
	}

	Vec2 operator -(Vec2 _vOther)
	{
		return Vec2(x - _vOther.x, y - _vOther.y);
	}

	Vec2 operator -()
	{
		return Vec2(-x, -y);
	}

	void operator -= (Vec2 _vOther)
	{
		x -= _vOther.x;
		y -= _vOther.y;
	}

	Vec2 operator /(Vec2 _vOther)
	{
		return Vec2(x / _vOther.x, y / _vOther.y);
	}

	void operator /= (Vec2 _vOther)
	{
		x /= _vOther.x;
		y /= _vOther.y;
	}

	Vec2 operator /(float _f)
	{
		return Vec2(x / _f, y / _f);
	}

	void operator /= (float _f)
	{
		x /= _f;
		y /= _f;
	}

	Vec2 operator *(Vec2 _vOther)
	{
		return Vec2(x * _vOther.x, y * _vOther.y);
	}

	void operator *= (Vec2 _vOther)
	{
		x *= _vOther.x;
		y *= _vOther.y;
	}

	Vec2 operator *(float _f)
	{
		return Vec2(x * _f, y * _f);
	}

	void operator *= (float _f)
	{
		x *= _f;
		y *= _f;
	}

	bool operator == (Vec2 _vOther)
	{
		if (x == _vOther.x && y == _vOther.y)
		{
			return true;
		}
		return false;
	}


public:
	Vec2()
		: x(0.f)
		, y(0.f)
	{}

	Vec2(float _x, float _y)
		: x(_x), y(_y)
	{}

	Vec2(UINT _x, UINT _y)
		: x((float)_x), y((float)_y)
	{}

	Vec2(POINT _pt)
		: x((float)_pt.x)
		, y((float)_pt.y)
	{}

	~Vec2()
	{}
};


struct tEvent
{
	EVENT_TYPE	eType;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
};

struct tAnimFrm
{
	Vec2  vLeftTop; // 프레임 자를 위치(좌상단)
	Vec2  vSize;	// 프레임 사이즈
	Vec2  vOffset;	// 추가 이동(프레임이 고르지 못할 경우)
	float fDuration;	// 해당 프레임 노출 시간
};

struct tMonsterInfo
{
	int m_iHP;

	float m_fSpeed;
	float m_fDetectRange;
	float m_fAttackRange;
	float m_fAttackDelay;
	float m_fDeadTime;
};