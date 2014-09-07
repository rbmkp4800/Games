#pragma once

#ifndef _extypes_def_vectors_math_
#define _extypes_def_vectors_math_

#include "extypes.vectors.h"

#ifndef _extypes_inc_stdmath_
#define _extypes_inc_stdmath_
#include <math.h>
#endif

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
inline float length(float32x2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}
inline float length(float32x3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length(float32x4& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
inline float32x2 normalize(float32x2& v)
{
	return v / length(v);
}
inline float32x3 normalize(float32x3& v)
{
	return v / length(v);
}
inline float32x4 normalize(float32x4& v)
{
	return v / length(v);
}

#endif