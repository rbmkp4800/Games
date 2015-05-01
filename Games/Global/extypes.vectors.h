#pragma once

#ifndef _extypes_def_vectors_
#define _extypes_def_vectors_

#include "extypes.h"

template <typename type>
struct varx2
{
	type x, y;

	inline varx2() = default;
	inline varx2(type _x, type _y) : x(_x), y(_y) {}
	inline void set(type _x, type _y) { x = _x; y = _y; }

	inline float aspect() { return (float) x / (float) y; }
	inline type multiply() { return x * y; }

	template <typename _type> inline bool operator == (const varx2<_type>& val) const
	{
		return x == val.x && y == val.y;
	}

	template <typename _type> inline varx2<type> operator + (const varx2<_type>& val) const
	{
		return varx2<type>(x + val.x, y + val.y);
	}
	template <typename _type> inline varx2<type> operator - (const varx2<_type>& val) const
	{
		return varx2<type>(x - val.x, y - val.y);
	}
	template <typename _type> inline varx2<type> operator * (const varx2<_type>& val) const
	{
		return varx2<type>(x * val.x, y * val.y);
	}
	template <typename _type> inline varx2<type> operator / (const varx2<_type>& val) const
	{
		return varx2<type>(x / val.x, y / val.y);
	}
	template <typename _type> inline varx2<type> operator * (const _type& val) const
	{
		return varx2<type>(x * val, y * val);
	}
	template <typename _type> inline varx2<type> operator / (const _type& val) const
	{
		return varx2<type>(x / val, y / val);
	}

	template <typename _type> inline varx2<type>& operator += (const varx2<_type>& val)
	{
		x += val.x; y += val.y;
		return *this;
	}
	template <typename _type> inline varx2<type>& operator -= (const varx2<_type>& val)
	{
		x -= val.x; y -= val.y;
		return *this;
	}
	template <typename _type> inline varx2<type>& operator *= (const varx2<_type>& val)
	{
		x *= val.x; y *= val.y;
		return *this;
	}
	template <typename _type> inline varx2<type>& operator /= (const varx2<_type>& val)
	{
		x /= val.x; y /= val.y;
		return *this;
	}
	template <typename _type> inline varx2<type>& operator *= (const _type& val)
	{
		x *= val; y *= val;
		return *this;
	}
	template <typename _type> inline varx2<type>& operator /= (const _type& val)
	{
		x /= val; y /= val;
		return *this;
	}
};

template <typename type>
struct varx3
{
	type x, y, z;

	inline varx3() = default;
	inline varx3(type _x, type _y, type _z) : x(_x), y(_y), z(_z) {}
	inline void set(type _x, type _y, type _z) { x = _x; y = _y; z = _z; }

	template <typename _type> inline bool operator == (const varx3<_type>& val) const
	{
		return x == val.x && y == val.y && z == val.z;
	}

	template <typename _type> inline varx3<type> operator + (const varx3<_type>& val) const
	{
		return varx3<type>(x + val.x, y + val.y, z + val.z);
	}
	template <typename _type> inline varx3<type> operator - (const varx3<_type>& val) const
	{
		return varx3<type>(x - val.x, y - val.y, z - val.z);
	}
	template <typename _type> inline varx3<type> operator * (const varx3<_type>& val) const
	{
		return varx3<type>(x * val.x, y * val.y, z * val.z);
	}
	template <typename _type> inline varx3<type> operator / (const varx3<_type>& val) const
	{
		return varx3<type>(x / val.x, y / val.y, z / val.z);
	}
	template <typename _type> inline varx3<type> operator * (const _type& val) const
	{
		return varx3<type>(x * val, y * val, z * val);
	}
	template <typename _type> inline varx3<type> operator / (const _type& val) const
	{
		return varx3<type>(x / val, y / val, z / val);
	}

	template <typename _type> inline varx3<type>& operator += (const varx3<_type>& val)
	{
		x += val.x; y += val.y; z += val.z;
		return *this;
	}
	template <typename _type> inline varx3<type>& operator -= (const varx3<_type>& val)
	{
		x -= val.x; y -= val.y; z -= val.z;
		return *this;
	}
	template <typename _type> inline varx3<type>& operator *= (const varx3<_type>& val)
	{
		x *= val.x; y *= val.y; z *= val.z;
		return *this;
	}
	template <typename _type> inline varx3<type>& operator /= (const varx3<_type>& val)
	{
		x /= val.x; y /= val.y; z /= val.z;
		return *this;
	}
	template <typename _type> inline varx3<type>& operator *= (const _type& val)
	{
		x *= val; y *= val; z *= val;
		return *this;
	}
	template <typename _type> inline varx3<type>& operator /= (const _type& val)
	{
		x /= val; y /= val; z /= val;
		return *this;
	}
};

template <typename type>
struct varx4
{
	type x, y, z, w;

	inline varx4() = default;
	inline varx4(type _x, type _y, type _z, type _w) : x(_x), y(_y), z(_z), w(_w) {}
	inline void set(type _x, type _y, type _z, type _w) { x = _x; y = _y; z = _z; w = _w; }

	template <typename _type> inline bool operator == (const varx4<_type>& val) const
	{
		return x == val.x && y == val.y && z == val.z && w == val.w;
	}

	template <typename _type> inline varx4<type> operator + (const varx4<_type>& val) const
	{
		return varx4<type>(x + val.x, y + val.y, z + val.z, w + val.w);
	}
	template <typename _type> inline varx4<type> operator - (const varx4<_type>& val) const
	{
		return varx4<type>(x - val.x, y - val.y, z - val.z, w - val.w);
	}
	template <typename _type> inline varx4<type> operator * (const varx4<_type>& val) const
	{
		return varx4<type>(x * val.x, y * val.y, z * val.z, w * val.w);
	}
	template <typename _type> inline varx4<type> operator / (const varx4<_type>& val) const
	{
		return varx4<type>(x / val.x, y / val.y, z / val.z, w / val.w);
	}
	template <typename _type> inline varx4<type> operator * (const _type& val) const
	{
		return varx4<type>(x * val, y * val, z * val, w * val);
	}
	template <typename _type> inline varx4<type> operator / (const _type& val) const
	{
		return varx4<type>(x / val, y / val, z / val, w / val);
	}

	template <typename _type> inline varx4<type>& operator += (const varx4<_type>& val)
	{
		x += val.x; y += val.y; z += val.z; w += val.w;
		return *this;
	}
	template <typename _type> inline varx4<type>& operator -= (const varx4<_type>& val)
	{
		x -= val.x; y -= val.y; z -= val.z; w -= val.w;
		return *this;
	}
	template <typename _type> inline varx4<type>& operator *= (const varx4<_type>& val)
	{
		x *= val.x; y *= val.y; z *= val.z; w *= val.w;
		return *this;
	}
	template <typename _type> inline varx4<type>& operator /= (const varx4<_type>& val)
	{
		x /= val.x; y /= val.y; z /= val.z; w /= val.w;
		return *this;
	}
	template <typename _type> inline varx4<type>& operator *= (const _type& val)
	{
		x *= val; y *= val; z *= val; w *= val;
		return *this;
	}
	template <typename _type> inline varx4<type>& operator /= (const _type& val)
	{
		x /= val; y /= val; z /= val; w /= val;
		return *this;
	}
};

template <typename type>
inline varx2<type> operator - (const varx2<type>& val)
{
	return varx2<type>(-val.x, -val.y);
}
template <typename type>
inline varx3<type> operator - (const varx3<type>& val)
{
	return varx3<type>(-val.x, -val.y, -val.z);
}
template <typename type>
inline varx4<type> operator - (const varx4<type>& val)
{
	return varx4<type>(-val.x, -val.y, -val.z, -val.w);
}

template <typename type>
struct rectvar
{
	type left, top, right, bottom;

	inline rectvar() = default;
	inline rectvar(type _left, type _top, type _right, type _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}

	inline void set(type _left, type _top, type _right, type _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
};

using uint8x2 = varx2<uint8>;
using uint8x3 = varx3<uint8>;
using uint8x4 = varx4<uint8>;
using uint16x2 = varx2<uint16>;
using uint16x3 = varx3<uint16>;
using uint16x4 = varx4<uint16>;
using uint32x2 = varx2<uint32>;
using uint32x3 = varx3<uint32>;
using uint32x4 = varx4<uint32>;
using uint64x2 = varx2<uint64>;
using uint64x3 = varx3<uint64>;
using uint64x4 = varx4<uint64>;

using sint8x2 = varx2<sint8>;
using sint8x3 = varx3<sint8>;
using sint8x4 = varx4<sint8>;
using sint16x2 = varx2<sint16>;
using sint16x3 = varx3<sint16>;
using sint16x4 = varx4<sint16>;
using sint32x2 = varx2<sint32>;
using sint32x3 = varx3<sint32>;
using sint32x4 = varx4<sint32>;
using sint64x2 = varx2<sint64>;
using sint64x3 = varx3<sint64>;
using sint64x4 = varx4<sint64>;

using float32x2 = varx2<float32>;
using float32x3 = varx3<float32>;
using float32x4 = varx4<float32>;
using float64x2 = varx2<float64>;
using float64x3 = varx3<float64>;
using float64x4 = varx4<float64>;

using rectu8 = rectvar<uint8>;
using rectu16 = rectvar<uint16>;
using rectu32 = rectvar<uint32>;
using rectu64 = rectvar<uint64>;
using rects8 = rectvar<sint8>;
using rects16 = rectvar<sint16>;
using rects32 = rectvar<sint32>;
using rects64 = rectvar<sint64>;
using rectf32 = rectvar<float32>;
using rectf64 = rectvar<float64>;

template <typename type>
inline type dot(const varx2<type>& a, const varx2<type>& b)
{
	return a.x * b.x + a.y * b.y;
}
template <typename type>
inline type dot(const varx3<type>& a, const varx3<type>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
template <typename type>
inline type dot(const varx4<type>& v1, const varx4<type>& v2)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <typename type>
inline varx3<type> cross(const varx3<type>& a, const varx3<type>& b)
{
	return varx3<type>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}
template <typename type>
inline type cross(const varx2<type>& a, const varx2<type>& b)
{
	return a.x * b.y - a.y * b.x;
}

#endif