#pragma once

#ifndef _extypes_def_vectors_math_
#define _extypes_def_vectors_math_

#ifndef _extypes_inc_stdmath_
#define _extypes_inc_stdmath_
#include <math.h>
#endif

struct float32x2
{
	float x, y;

	inline float32x2() = default;
	inline float32x2(float _x, float _y) : x(_x), y(_y) {}

	inline void set(float _x, float _y) { x = _x; y = _y; }

	inline float32x2 operator * (float mul)			{ return float32x2(x * mul, y * mul); }
	inline float32x2 operator *= (float mul)		{ return float32x2(x *= mul, y *= mul); }
	inline float32x2 operator / (float div)			{ return float32x2(x / div, y / div); }
	inline float32x2 operator /= (float div)		{ return float32x2(x /= div, y /= div); }
	inline float32x2 operator * (float32x2& mul)	{ return float32x2(x * mul.x, y * mul.y); }
	inline float32x2 operator *= (float32x2& mul)	{ return float32x2(x *= mul.x, y *= mul.y); }
	inline float32x2 operator + (float32x2& add)	{ return float32x2(x + add.x, y + add.y); }
	inline float32x2 operator += (float32x2& add)	{ return float32x2(x += add.x, y += add.y); }
	inline float32x2 operator - (float32x2& sub)	{ return float32x2(x - sub.x, y - sub.y); }
	inline float32x2 operator -= (float32x2& sub)	{ return float32x2(x -= sub.x, y -= sub.y); }
};

struct float32x3
{
	float x, y, z;

	inline float32x3() = default;
	inline float32x3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	inline void set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

	inline float32x3 operator * (float mul)			{ return float32x3(x * mul, y * mul, z * mul); }
	inline float32x3 operator *= (float mul)		{ return float32x3(x *= mul, y *= mul, z *= mul); }
	inline float32x3 operator / (float div)			{ return float32x3(x / div, y / div, z / div); }
	inline float32x3 operator /= (float div)		{ return float32x3(x /= div, y /= div, z /= div); }
	inline float32x3 operator * (float32x3& mul)	{ return float32x3(x * mul.x, y * mul.y, z * mul.z); }
	inline float32x3 operator *= (float32x3& mul)	{ return float32x3(x *= mul.x, y *= mul.y, z *= mul.z); }
	inline float32x3 operator + (float32x3& add)	{ return float32x3(x + add.x, y + add.y, z + add.z); }
	inline float32x3 operator += (float32x3& add)	{ return float32x3(x += add.x, y += add.y, z += add.z); }
	inline float32x3 operator - (float32x3& sub)	{ return float32x3(x - sub.x, y - sub.y, z - sub.z); }
	inline float32x3 operator -= (float32x3& sub)	{ return float32x3(x -= sub.x, y -= sub.y, z -= sub.z); }
};

struct float32x4
{
	float x, y, z, w;

	inline float32x4() = default;
	inline float32x4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	inline void set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};

inline float32x2 operator - (float32x2& arg)
{
	return float32x2(-arg.x, -arg.y);
}
inline float32x3 operator - (float32x3& arg)
{
	return float32x3(-arg.x, -arg.y, -arg.z);
}
inline float32x4 operator - (float32x4& arg)
{
	return float32x4(-arg.x, -arg.y, -arg.z, -arg.w);
}

inline float dot(float32x2& v1, float32x2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
inline float dot(float32x3& v1, float32x3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline float32x3 cross(float32x3& v1, float32x3& v2)
{
	float32x3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}
inline float32x2 normalize(float32x2& arg)
{
	arg.normalize();
	return arg;
}
inline float32x3 normalize(float32x3& arg)
{
	arg.normalize();
	return arg;
}

typedef float32x2 f32x2;
typedef float32x3 f32x3;
typedef float32x4 f32x4;

#endif