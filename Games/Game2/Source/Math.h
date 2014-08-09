#include <math.h>
#include <memory.h>

#include "types.h"

class Matrix4x4
{
public:
	union
	{
		float data[4][4];
		uint32 dataui[4][4];
	};

	inline void Clear()
	{
		memset(data, 0, sizeof(float) * 16);
	}

	inline void SetAsTransposed()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < i; j++)
				xorswap(dataui[i][j], dataui[j][i]);
	}
	inline void SetAsIdentity()
	{
		Clear();
		data[0][0] = 1.0f;
		data[1][1] = 1.0f;
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
	}
	inline void SetAsScale(float x, float y, float z)
	{
		Clear();
		data[0][0] = x;
		data[1][1] = y;
		data[2][2] = z;
		data[3][3] = 1.0f;
	}
	inline void SetAsRotateX(float angle)
	{
		Clear();
		float angleSin = sinf(angle), angleCos = cosf(angle);
		data[0][0] = 1.0f;
		data[1][1] = angleCos;
		data[1][2] = angleSin;
		data[2][1] = -angleSin;
		data[2][2] = angleCos;
		data[3][3] = 1.0f;
	}
	inline void SetAsRotateY(float angle)
	{
		Clear();
		float angleSin = sinf(angle), angleCos = cosf(angle);
		data[0][0] = angleCos;
		data[1][1] = 1.0f;
		data[2][2] = angleCos;
		data[0][2] = angleSin;
		data[2][0] = -angleSin;
	}
	inline void SetAsRotateZ(float angle)
	{
		Clear();
		float angleSin = sinf(angle), angleCos = cosf(angle);
		data[0][0] = angleCos;
		data[1][1] = angleCos;
		data[0][1] = angleSin;
		data[1][0] = -angleSin;
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
	}
	inline void SetAsTranslate(float x, float y, float z)
	{
		Clear();
		data[0][0] = 1.0f;
		data[1][1] = 1.0f;
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
		data[3][0] = x;
		data[3][1] = y;
		data[3][2] = z;
	}
	inline void SetAsPerspective(float fov, float aspect, float zNear, float zFar)
	{
		float yScale = 1.0f / tanf(fov / 2.0f);
		float xScale = yScale / aspect;

		Clear();
		data[0][0] = xScale;
		data[1][1] = yScale;
		data[2][2] = zFar / (zFar - zNear);
		data[2][3] = 1.0f;
		data[3][2] = -zNear * zFar / (zFar - zNear);
	}
	inline void SetAsLookAt(float32x3 eye, float32x3 at, float32x3 up)
	{
		float32x3 xaxis;
		float32x3 yaxis;
		float32x3 zaxis = eye - at;
		zaxis.normalize();
		xaxis = cross(up, zaxis);
		xaxis.normalize();
		yaxis = cross(zaxis, xaxis);

		data[0][0] = xaxis.x;
		data[0][1] = yaxis.x;
		data[0][2] = zaxis.x;
		data[0][3] = 0.0f;

		data[1][0] = xaxis.y;
		data[1][1] = yaxis.y;
		data[1][2] = zaxis.y;
		data[1][3] = 0.0f;

		data[2][0] = xaxis.z;
		data[2][1] = yaxis.z;
		data[2][2] = zaxis.z;
		data[2][3] = 0.0f;

		data[3][0] = dot(xaxis, eye);
		data[3][1] = dot(yaxis, eye);
		data[3][2] = dot(zaxis, eye);
		data[3][3] = 1.0f;
	}

	static inline Matrix4x4 Transpose()
	{
		Matrix4x4 result;
		result.SetAsTransposed();
		return result;
	}
	static inline Matrix4x4 Identity()
	{
		Matrix4x4 result;
		result.SetAsIdentity();
		return result;
	}
	static inline Matrix4x4 Scale(float x, float y, float z)
	{
		Matrix4x4 result;
		result.SetAsScale(x, y, z);
		return result;
	}
	static inline Matrix4x4 RotateX(float angle)
	{
		Matrix4x4 result;
		result.SetAsRotateX(angle);
		return result;
	}
	static inline Matrix4x4 RotateY(float angle)
	{
		Matrix4x4 result;
		result.SetAsRotateY(angle);
		return result;
	}
	static inline Matrix4x4 RotateZ(float angle)
	{
		Matrix4x4 result;
		result.SetAsRotateZ(angle);
		return result;
	}
	static inline Matrix4x4 Translate(float x, float y, float z)
	{
		Matrix4x4 result;
		result.SetAsTranslate(x, y, z);
		return result;
	}
	static inline Matrix4x4 Perspective(float fov, float aspect, float zNear, float zFar)
	{
		Matrix4x4 result;
		result.SetAsPerspective(fov, aspect, zNear, zFar);
		return result;
	}
	static inline Matrix4x4 LookAt(float32x3 eye, float32x3 at, float32x3 up)
	{
		Matrix4x4 result;
		result.SetAsLookAt(eye, at, up);
		return result;
	}
};

inline Matrix4x4 operator * (Matrix4x4& matr1, Matrix4x4& matr2)
{
	Matrix4x4 result;
	result.Clear();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				result.data[j][i] += matr1.data[k][i] * matr2.data[j][k];
	return result;
}
inline float32x4 operator * (float32x3& vect, Matrix4x4& matr)
{
	float32x4 result;
	result.x = vect.x * matr.data[0][0] + vect.y * matr.data[1][0] + vect.z * matr.data[2][0] + matr.data[3][0];
	result.y = vect.x * matr.data[0][1] + vect.y * matr.data[1][1] + vect.z * matr.data[2][1] + matr.data[3][1];
	result.z = vect.x * matr.data[0][2] + vect.y * matr.data[1][2] + vect.z * matr.data[2][2] + matr.data[3][2];
	result.w = vect.x * matr.data[0][3] + vect.y * matr.data[1][3] + vect.z * matr.data[2][3] + matr.data[3][3];
	return result;
}