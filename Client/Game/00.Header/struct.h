#pragma once

#include <type_traits>

struct Vec2
{
	float x;
	float y;


public:
	// 길이를 1로. 0벡터는 방향이 없으므로 그대로 둔다 (예전에는 0으로 나눠 NaN이 위치값으로 퍼졌다)
	void Normalize()
	{
		const float fDist = sqrtf(x * x + y * y);
		if (fDist <= 0.f)
			return;
		x /= fDist;
		y /= fDist;
	}

	// 정확히 0인가 (값을 0으로 둔 것을 확인할 때. 계산 결과를 비교할 때는 길이와 허용 오차로)
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

	// operator==는 두지 않는다 (부동소수를 ==로 비교하던 것. 쓰는 곳도 없었다)

public:
	constexpr Vec2()
		: x(0.f)
		, y(0.f)
	{}

	constexpr Vec2(const float _x, const float _y)
		: x(_x), y(_y)
	{}

	// 같은 정수 형 두 개 (int, UINT 등). 예전에는 UINT 생성자만 있어 Vec2(1, 2)가 float·UINT 사이에서 모호했다
	template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	constexpr Vec2(const T _x, const T _y)
		: x(static_cast<float>(_x)), y(static_cast<float>(_y))
	{}

	Vec2(const POINT _pt)
		: x(static_cast<float>(_pt.x))
		, y(static_cast<float>(_pt.y))
	{}

	// 소멸자를 따로 두지 않는다 (빈 소멸자가 있으면 이동 생성·이동 대입이 만들어지지 않는다)
};


struct tEvent
{
	EVENT_TYPE	eType = EVENT_TYPE::CREATE_OBJECT;
	DWORD_PTR	wParam = 0;
	DWORD_PTR	lParam = 0;
};

struct tAnimFrm
{
	Vec2  vLeftTop; // 잘라낼 자리 위치(좌상단)
	Vec2  vSize;	// 잘라낼 사이즈
	Vec2  vOffset;	// 추가 이동(오프셋 적용 시 위치 보정용)
	float fDuration = 0.f;	// 해당 프레임 재생 시간
};

