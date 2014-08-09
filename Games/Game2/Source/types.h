#pragma once

#include <math.h>

#define elemcntof(arr)	(sizeof(arr) / sizeof(*arr))
#define xorswap(a1, a2)	((a1 ^= a2), (a2 ^= a1), (a1 ^= a2))
#define sqr(a1)			((a1) * (a1))
#define absf(val)		((val < 0.0f) ? (-val) : (val))

typedef char int8;
typedef short int int16;
typedef int int32;
typedef long long int int64;
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;
typedef float float32;
typedef double float64;

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif


#ifndef UINT8_MAX
#define UINT8_MAX	0xff
#endif

#ifndef UINT16_MAX
#define UINT16_MAX	0xffff
#endif

#ifndef UINT32_MAX
#define UINT32_MAX	0xfffffffful
#endif

#ifndef UINT64_MAX
#define UINT64_MAX	0xffffffffffffffffull
#endif

struct uint32x2
{
	uint32 x, y;

	inline uint32x2() {}
	inline uint32x2(uint32 _x, uint32 _y) : x(_x), y(_y) {}

	inline float aspect() { return (float) x / (float) y; }
};
struct int32x2
{
	int32 x, y;

	inline int32x2() {}
	inline int32x2(int32 _x, int32 _y) : x(_x), y(_y) {}

	inline float aspect() { return (float) x / (float) y; }
};

struct uint32x3
{
	uint32 x, y, z;

	inline uint32x3() {}
	inline uint32x3(uint32 _x, uint32 _y, uint32 _z) : x(_x), y(_y), z(_z) {}
};

struct uint32x4
{
	uint32 x, y, z, w;

	inline uint32x4() {}
	inline uint32x4(uint32 _x, uint32 _y, uint32 _z, uint32 _w) : x(_x), y(_y), z(_z), w(_w) {}
};

__declspec(align(2)) struct uint16x3
{
	uint16 x, y, z;

	inline uint16x3() {}
	inline uint16x3(uint16 _x, uint16 _y, uint16 _z) : x(_x), y(_y), z(_z) {}

	inline void set(uint16 _x, uint16 _y, uint16 _z) { x = _x; y = _y; z = _z; }
};

struct uint16x4
{
	uint16 x, y, z, w;

	inline uint16x4() {}
	inline uint16x4(uint16 _x, uint16 _y, uint16 _z, uint16 _w) : x(_x), y(_y), z(_z), w(_w) {}

	inline void set(uint16 _x, uint16 _y, uint16 _z, uint16 _w) { x = _x; y = _y; z = _z; w = _w; }
};

struct uint16x2
{
	uint16 x, y;

	inline uint16x2() {}
	inline uint16x2(uint16 _x, uint16 _y) : x(_x), y(_y) {}

	inline void set(uint16 _x, uint16 _y) { x = _x; y = _y; }

	inline float aspect() { return (float) x / (float) y; }
	inline uint32 multiply() { return x * y; }
};

struct int16x2
{
	int16 x, y;

	inline int16x2() {}
	inline int16x2(int16 _x, int16 _y) : x(_x), y(_y) {}

	inline void set(int16 _x, int16 _y) { x = _x; y = _y; }

	inline int16x2 add(int16 _x, int16 _y) { return int16x2(x + _x, y + _y); }
	inline float aspect() { return (float) x / (float) y; }
	inline int32 multiply() { return x * y; }
};

__declspec(align(2)) struct uint8x2
{
	uint8 x, y;

	inline uint8x2() {}
	inline uint8x2(uint8 _x, uint8 _y) : x(_x), y(_y) {}

	inline void set(uint8 _x, uint8 _y) { x = _x; y = _y; }
};

struct float32x2
{
	float x, y;

	inline float32x2() {}
	inline float32x2(float _x, float _y) : x(_x), y(_y) {}

	inline void set(float _x, float _y) { x = _x; y = _y; }

	inline float32x2 operator * (float mul)			{ return float32x2(x * mul, y * mul); }
	inline float32x2 operator *= (float mul)		{ return float32x2(x *= mul, y *= mul); }
	inline float32x2 operator / (float div)			{ return float32x2(x / div, y / div); }
	inline float32x2 operator /= (float div)		{ return float32x2(x /= div, y /= div); }
	inline float32x2 operator * (float32x2 mul)		{ return float32x2(x * mul.x, y * mul.y); }
	inline float32x2 operator *= (float32x2 mul)	{ return float32x2(x *= mul.x, y *= mul.y); }
	inline float32x2 operator + (float32x2 add)		{ return float32x2(x + add.x, y + add.y); }
	inline float32x2 operator += (float32x2 add)	{ return float32x2(x += add.x, y += add.y); }
	inline float32x2 operator - (float32x2 sub)		{ return float32x2(x - sub.x, y - sub.y); }
	inline float32x2 operator -= (float32x2 sub)	{ return float32x2(x -= sub.x, y -= sub.y); }

	inline float32x2 operator += (float add)
	{
		float tmp = this->length();
		tmp = (tmp + add) / tmp;
		x *= tmp;
		y *= tmp;
		return *this;
	}
	inline float32x2 operator -= (float sub)
	{
		float tmp = this->length();
		tmp = (tmp - sub) / tmp;
		x *= tmp;
		y *= tmp;
		return *this;
	}

	inline float length() { return sqrtf(x * x + y * y); }
	inline void normalize()
	{
		float length = this->length();
		if (length < 0.000001f) length = 0.000001f;
		x /= length; y /= length;
	}
};

struct float32x3
{
	float x, y, z;

	inline float32x3() {}
	inline float32x3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	inline void set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

	inline float32x3 operator * (float mul)			{ return float32x3(x * mul, y * mul, z * mul); }
	inline float32x3 operator *= (float mul)		{ return float32x3(x *= mul, y *= mul, z *= mul); }
	inline float32x3 operator / (float div)			{ return float32x3(x / div, y / div, z / div); }
	inline float32x3 operator /= (float div)		{ return float32x3(x /= div, y /= div, z /= div); }
	inline float32x3 operator * (float32x3 mul)		{ return float32x3(x * mul.x, y * mul.y, z * mul.z); }
	inline float32x3 operator *= (float32x3 mul)	{ return float32x3(x *= mul.x, y *= mul.y, z *= mul.z); }
	inline float32x3 operator + (float32x3 add)		{ return float32x3(x + add.x, y + add.y, z + add.z); }
	inline float32x3 operator += (float32x3 add)	{ return float32x3(x += add.x, y += add.y, z += add.z); }
	inline float32x3 operator - (float32x3 sub)		{ return float32x3(x - sub.x, y - sub.y, z - sub.z); }
	inline float32x3 operator -= (float32x3 sub)	{ return float32x3(x -= sub.x, y -= sub.y, z -= sub.z); }
	inline float32x3 operator += (float add)
	{
		float tmp = this->length();
		tmp = (tmp + add) / tmp;
		x *= tmp;
		y *= tmp;
		z *= tmp;
		return *this;
	}
	inline float32x3 operator -= (float sub)
	{
		float tmp = this->length();
		tmp = (tmp - sub) / tmp;
		x *= tmp;
		y *= tmp;
		z *= tmp;
		return *this;
	}

	inline float length() { return sqrtf(x * x + y * y + z * z); }
	inline void normalize()
	{
		float length = this->length();
		if (length < 0.000001f) length = 0.000001f;
		x /= length; y /= length; z /= length;
	}
};

struct float32x4
{
	float x, y, z, w;

	inline float32x4() {}
	inline float32x4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	inline void set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

	inline float length() { return sqrtf(x * x + y * y + z * z + w * w); }
};

struct rectf32
{
	float left, top, right, bottom;

	inline rectf32() {}
	inline rectf32(float _left, float _top, float _right, float _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}

	inline void set(float _left, float _top, float _right, float _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }

	static inline rectf32 unitRect() { return rectf32(0.0f, 0.0f, 1.0f, 1.0f); }
};

inline float32x2 operator - (float32x2 arg)
{
	return float32x2(-arg.x, -arg.y);
}
inline float32x3 operator - (float32x3 arg)
{
	return float32x3(-arg.x, -arg.y, -arg.z);
}

inline float dot(const float32x2& v1, const float32x2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
inline float dot(const float32x3& v1, const float32x3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline float32x3 cross(float32x3 v1, float32x3 v2)
{
	float32x3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}
inline float32x2 normalize(float32x2 arg)
{
	arg.normalize();
	return arg;
}
inline float32x3 normalize(float32x3 arg)
{
	arg.normalize();
	return arg;
}