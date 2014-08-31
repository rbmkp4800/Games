#pragma once

#ifndef _extypes_def_vectors_
#define _extypes_def_vectors_

struct uint32x2
{
	uint32 x, y;

	inline uint32x2() = default;
	inline uint32x2(uint32 _x, uint32 _y) : x(_x), y(_y) {}
	inline void set(uint32 _x, uint32 _y) { x = _x; y = _y; }

	inline float aspect() { return (float) x / (float) y; }
};
struct int32x2
{
	int32 x, y;

	inline int32x2() = default;
	inline int32x2(int32 _x, int32 _y) : x(_x), y(_y) {}
	inline void set(int32 _x, int32 _y) { x = _x; y = _y; }

	inline float aspect() { return (float) x / (float) y; }
};

struct uint32x3
{
	uint32 x, y, z;

	inline uint32x3() {} = default;
	inline uint32x3(uint32 _x, uint32 _y, uint32 _z) : x(_x), y(_y), z(_z) {}
};

struct uint32x4
{
	uint32 x, y, z, w;

	inline uint32x4() = default;
	inline uint32x4(uint32 _x, uint32 _y, uint32 _z, uint32 _w) : x(_x), y(_y), z(_z), w(_w) {}
};

__declspec(align(2)) struct uint16x3
{
	uint16 x, y, z;

	inline uint16x3() = default;
	inline uint16x3(uint16 _x, uint16 _y, uint16 _z) : x(_x), y(_y), z(_z) {}
	inline void set(uint16 _x, uint16 _y, uint16 _z) { x = _x; y = _y; z = _z; }
};

struct uint16x4
{
	uint16 x, y, z, w;

	inline uint16x4() = default;
	inline uint16x4(uint16 _x, uint16 _y, uint16 _z, uint16 _w) : x(_x), y(_y), z(_z), w(_w) {}
	inline void set(uint16 _x, uint16 _y, uint16 _z, uint16 _w) { x = _x; y = _y; z = _z; w = _w; }
};

struct uint16x2
{
	uint16 x, y;

	inline uint16x2() = default;
	inline uint16x2(uint16 _x, uint16 _y) : x(_x), y(_y) {}
	inline void set(uint16 _x, uint16 _y) { x = _x; y = _y; }

	inline float aspect() { return (float) x / (float) y; }
	inline uint32 multiply() { return x * y; }
};

struct int16x2
{
	int16 x, y;

	inline int16x2() = default;
	inline int16x2(int16 _x, int16 _y) : x(_x), y(_y) {}

	inline void set(int16 _x, int16 _y) { x = _x; y = _y; }

	inline int16x2 add(int16 _x, int16 _y) { return int16x2(x + _x, y + _y); }
	inline float aspect() { return (float) x / (float) y; }
	inline int32 multiply() { return x * y; }
};

__declspec(align(2)) struct uint8x2
{
	uint8 x, y;

	inline uint8x2() = default;
	inline uint8x2(uint8 _x, uint8 _y) : x(_x), y(_y) {}

	inline void set(uint8 _x, uint8 _y) { x = _x; y = _y; }
};

struct rectf32
{
	float left, top, right, bottom;

	inline rectf32() = default;
	inline rectf32(float _left, float _top, float _right, float _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}

	inline void set(float _left, float _top, float _right, float _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
};
struct rectu32
{
	uint32 left, top, right, bottom;

	inline rectu32() = default;
	inline rectu32(uint32 _left, uint32 _top, uint32 _right, uint32 _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}

	inline void set(uint32 _left, uint32 _top, uint32 _right, uint32 _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
};
struct rects32
{
	int32 left, top, right, bottom;

	inline rects32() = default;
	inline rects32(int32 _left, int32 _top, int32 _right, int32 _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}

	inline void set(int32 _left, int32 _top, int32 _right, int32 _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
};

#endif