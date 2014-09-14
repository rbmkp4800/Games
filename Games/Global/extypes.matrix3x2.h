#pragma once

#ifndef _extypes_def_matrix3x2_
#define _extypes_def_matrix3x2_

#include "extypes.vectors.h"

#ifndef _extypes_inc_stdmath_
#define _extypes_inc_stdmath_
#include <math.h>
#endif

struct matrix3x2
{
	float data[3][2];

	inline void clear()
	{
		data[0][0] = 0.0f;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = 0.0f;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsIdentity()
	{
		clear();
		data[0][0] = 1.0f;
		data[1][1] = 1.0f;
	}
	inline void setAsTranslation(float x, float y)
	{
		setAsIdentity();
		data[2][0] = x;
		data[2][1] = y;
	}
	inline void setAsScale(float scale)
	{
		clear();
		data[0][0] = scale;
		data[1][1] = scale;
	}
	inline void setAsScale(float xs, float ys)
	{
		clear();
		data[0][0] = xs;
		data[1][1] = ys;
	}
	inline void setAsScale(float xs, float ys, float x, float y)
	{
		data[0][0] = xs;
		data[1][1] = ys;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[2][0] = x * (1.0f - xs);
		data[2][1] = y * (1.0f - ys);
	}
	inline void setAsRotation(float angle)
	{
		data[0][0] = data[1][1] = cosf(angle);
		data[0][1] = -(data[1][0] = sinf(angle));
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsRotation(float x, float y, float angle)
	{
		setAsRotation(angle);
		data[2][0] = x * (1.0f - cosf(angle)) - sinf(angle) * y;
		data[2][1] = y * (1.0f - cosf(angle)) + sinf(angle) * x;
	}
	inline void setAsVerticalReflection()
	{
		clear();
		data[0][0] = 1.0f;
		data[1][1] = -1.0f;
	}
	inline void setAsHorizontalReflection()
	{
		clear();
		data[0][0] = -1.0f;
		data[1][1] = 1.0f;
	}
	inline void setAsReflection(float x, float y)
	{
		float d = sqrval(x) + sqrval(y);
		float m = 2.0f * x * y / d;
		float s = (sqrval(x) - sqrval(y)) / d;

		clear();
		data[0][0] = s;
		data[0][1] = m;
		data[1][0] = m;
		data[1][1] = -s;
	}

	inline void setAsTranslation(float32x2 translation)
	{
		setAsTranslation(translation.x, translation.y);
	}
	inline void setAsScale(float32x2 scale)
	{
		setAsScale(scale.x, scale.y);
	}
	inline void setAsScale(float32x2 scale, float32x2 position)
	{
		setAsScale(scale.x, scale.y, position.x, position.y);
	}
	inline void setAsRotation(float32x2 position, float angle)
	{
		setAsRotation(position.x, position.y, angle);
	}
	inline void setAsReflection(float32x2 reflection)
	{
		setAsReflection(reflection.x, reflection.y);
	}

	static inline matrix3x2 identity()
	{
		matrix3x2 matrix;
		matrix.setAsIdentity();
		return matrix;
	}
	static inline matrix3x2 translation(float x, float y)
	{
		matrix3x2 result;
		result.setAsTranslation(x, y);
		return result;
	}
	static inline matrix3x2 translation(float32x2 translation)
	{
		matrix3x2 result;
		result.setAsTranslation(translation);
		return result;
	}
	static inline matrix3x2 scale(float scale)
	{
		matrix3x2 matrix;
		matrix.setAsScale(scale);
		return matrix;
	}
	static inline matrix3x2 scale(float x, float y)
	{
		matrix3x2 matrix;
		matrix.setAsScale(x, y);
		return matrix;
	}
	static inline matrix3x2 scale(float32x2 scale)
	{
		matrix3x2 matrix;
		matrix.setAsScale(scale);
		return matrix;
	}
	static inline matrix3x2 rotation(float angle)
	{
		matrix3x2 matrix;
		matrix.setAsRotation(angle);
		return matrix;
	}
	static inline matrix3x2 rotation(float x, float y, float angle)
	{
		matrix3x2 matrix;
		matrix.setAsRotation(x, y, angle);
		return matrix;
	}
	static inline matrix3x2 rotation(float32x2 position, float angle)
	{
		matrix3x2 matrix;
		matrix.setAsRotation(position, angle);
		return matrix;
	}
	static inline matrix3x2 verticalReflection()
	{
		matrix3x2 matrix;
		matrix.setAsVerticalReflection();
		return matrix;
	}
	static inline matrix3x2 horizontalReflection()
	{
		matrix3x2 matrix;
		matrix.setAsHorizontalReflection();
		return matrix;
	}
	static inline matrix3x2 reflection(float x, float y)
	{
		matrix3x2 matrix;
		matrix.setAsReflection(x, y);
		return matrix;
	}
	static inline matrix3x2 reflection(float32x2 reflection)
	{
		matrix3x2 matrix;
		matrix.setAsReflection(reflection);
		return matrix;
	}
};

inline float32x2 operator * (float32x2 vector, matrix3x2 matrix)
{
	return float32x2(
		matrix.data[0][0] * vector.x + matrix.data[1][0] * vector.y + matrix.data[2][0],
		matrix.data[0][1] * vector.x + matrix.data[1][1] * vector.y + matrix.data[2][1]);
}
inline matrix3x2 operator * (matrix3x2 a, matrix3x2 b)
{
	matrix3x2 result;
	result.data[0][0] = a.data[0][0] * b.data[0][0] + a.data[1][0] * b.data[0][1];
	result.data[0][1] = a.data[0][1] * b.data[0][0] + a.data[1][1] * b.data[0][1];
	result.data[1][0] = a.data[0][0] * b.data[1][0] + a.data[1][0] * b.data[1][1];
	result.data[1][1] = a.data[0][1] * b.data[1][0] + a.data[1][1] * b.data[1][1];
	result.data[2][0] = a.data[0][0] * b.data[2][0] + a.data[1][0] * b.data[2][1] + a.data[2][0];
	result.data[2][1] = a.data[0][1] * b.data[2][0] + a.data[1][1] * b.data[2][1] + a.data[2][1];
	return result;
}

#endif