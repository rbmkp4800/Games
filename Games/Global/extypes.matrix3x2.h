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
		data[0][0] = 1.0f;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = 1.0f;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsTranslation(float x, float y)
	{
		data[0][0] = 1.0f;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = 1.0f;
		data[2][0] = x;
		data[2][1] = y;
	}
	inline void setAsScale(float scale)
	{
		data[0][0] = scale;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = scale;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsScale(float xscale, float yscale)
	{
		data[0][0] = xscale;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = yscale;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsScale(float xs, float ys, float x, float y)
	{
		data[0][0] = xs;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = ys;
		data[2][0] = x * (1.0f - xs);
		data[2][1] = y * (1.0f - ys);
	}
	inline void setAsTranslationAndScale(float xtrans, float ytrans, float xscale, float yscale)
	{
		data[0][0] = xscale;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = yscale;
		data[2][0] = xtrans;
		data[2][1] = ytrans;
	}
	inline void setAsRotation(float angle)
	{
		float a = cosf(angle);
		float b = sinf(angle);
		data[0][0] = a;
		data[0][1] = -b;
		data[1][0] = b;
		data[1][1] = a;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsRotation(float x, float y, float angle)
	{
		float a = cosf(angle);
		float b = sinf(angle);
		data[0][0] = a;
		data[0][1] = -b;
		data[1][0] = b;
		data[1][1] = a;
		data[2][0] = x * (1.0f - a) - b * y;
		data[2][1] = y * (1.0f - a) + b * x;
	}
	inline void setAsVerticalReflection()
	{
		data[0][0] = 1.0f;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = -1.0f;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsHorizontalReflection()
	{
		data[0][0] = -1.0f;
		data[0][1] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = 1.0f;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
	}
	inline void setAsReflection(float x, float y)
	{
		float d = sqrval(x) + sqrval(y);
		float m = 2.0f * x * y / d;
		float s = (sqrval(x) - sqrval(y)) / d;

		data[0][0] = s;
		data[0][1] = m;
		data[1][0] = m;
		data[1][1] = -s;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
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
	static inline matrix3x2 translationAndScale(float xtrans, float ytrans, float xscale, float yscale)
	{
		matrix3x2 matrix;
		matrix.setAsTranslationAndScale(xtrans, ytrans, xscale, yscale);
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

inline float32x2 operator * (const float32x2& vector, const matrix3x2& matrix)
{
	return float32x2(
		matrix.data[0][0] * vector.x + matrix.data[1][0] * vector.y + matrix.data[2][0],
		matrix.data[0][1] * vector.x + matrix.data[1][1] * vector.y + matrix.data[2][1]);
}
inline matrix3x2 operator * (const matrix3x2& a, const matrix3x2& b)
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