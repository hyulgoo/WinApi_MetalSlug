#pragma once

struct Vec2
{
	float x;
	float y;


public:
	void Normalize() // 정규화
	{
		const float fDist = sqrtf(x * x + y * y);
		x /= fDist;
		y /= fDist;
	}

	bool IsZero() const
    {
		return x == 0.f && y == 0.f;
	}

	float Length() const
    {
		return sqrtf(x * x + y * y);
	}

	Vec2 operator +(const Vec2 _vOther) const
    {
		return Vec2(x + _vOther.x, y + _vOther.y);
	}

	void operator += (const Vec2 _vOther)
	{
		x += _vOther.x;
		y += _vOther.y;
	}

	Vec2 operator -(const Vec2 _vOther) const
    {
		return Vec2(x - _vOther.x, y - _vOther.y);
	}

	Vec2 operator -() const
    {
		return Vec2(-x, -y);
	}

	void operator -= (const Vec2 _vOther)
	{
		x -= _vOther.x;
		y -= _vOther.y;
	}

	Vec2 operator /(const Vec2 _vOther) const
    {
		return Vec2(x / _vOther.x, y / _vOther.y);
	}

	void operator /= (const Vec2 _vOther)
	{
		x /= _vOther.x;
		y /= _vOther.y;
	}

	Vec2 operator /(const float _f) const
    {
		return Vec2(x / _f, y / _f);
	}

	void operator /= (const float _f)
	{
		x /= _f;
		y /= _f;
	}

	Vec2 operator *(const Vec2 _vOther) const
    {
		return Vec2(x * _vOther.x, y * _vOther.y);
	}

	void operator *= (const Vec2 _vOther)
	{
		x *= _vOther.x;
		y *= _vOther.y;
	}

	Vec2 operator *(const float _f) const
    {
		return Vec2(x * _f, y * _f);
	}

	void operator *= (const float _f)
	{
		x *= _f;
		y *= _f;
	}

	bool operator == (const Vec2 _vOther) const
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

	Vec2(const float _x, const float _y)
		: x(_x), y(_y)
	{}

	Vec2(const UINT _x, const UINT _y)
		: x(static_cast<float>(_x)), y(static_cast<float>(_y))
	{}

	Vec2(const POINT _pt)
		: x(static_cast<float>(_pt.x))
		, y(static_cast<float>(_pt.y))
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
	Vec2  vLeftTop; // 잘라낼 자리 위치(좌상단)
	Vec2  vSize;	// 잘라낼 사이즈
	Vec2  vOffset;	// 추가 이동(오프셋 적용 시 위치 보정용)
	float fDuration;	// 해당 프레임 재생 시간
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