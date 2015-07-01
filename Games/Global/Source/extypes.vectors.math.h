#pragma once

#ifndef _extypes_def_vectors_math_
#define _extypes_def_vectors_math_

#include "extypes.vectors.h"

#ifndef _extypes_inc_stdmath_
#define _extypes_inc_stdmath_
#include <math.h>
#endif

inline float length(const float32x2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}
inline float length(const float32x3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length(const float32x4& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
inline float32x2 normalize(const float32x2& v)
{
	return v / length(v);
}
inline float32x3 normalize(const float32x3& v)
{
	return v / length(v);
}
inline float32x4 normalize(const float32x4& v)
{
	return v / length(v);
}

#endif