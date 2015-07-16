#pragma once

#ifndef _extypes_def_extypes_
#define _extypes_def_extypes_

using uint8 = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned long int;
using uint64 = unsigned long long int;
using sint8 = signed char;
using sint16 = signed short int;
using sint32 = signed long int;
using sint64 = signed long long int;
using float32 = float;
using float64 = double;

#ifdef _WIN64
using uintptr = uint64;
#else
using uintptr = __w64 uint32;
#endif

template <typename type>
void xorswap(type& val1, type& val2)
{
	val1 ^= val2;
	val2 ^= val1;
	val1 ^= val2;
}

template <typename type, uintptr size> constexpr uintptr elemcntof(type(&)[size]) { return size; }

template <typename type>
constexpr inline type clamp(type val, type _min, type _max)
{
	if (val < _min) val = _min;
	if (val > _max) val = _max;
	return val;
}
template <typename type> constexpr inline type saturate(type val) { return clamp(val, type(0), type(1)); }
template <typename vectorType, typename coefType> constexpr inline vectorType lerp(const vectorType& x, const vectorType& y, coefType coef) { return x + coef * (y - x); }
template <typename type> constexpr inline type lincoef(type a, type b, type coef) { return (a - coef) / (a - b); }
template <typename type> constexpr inline type lincoefsatur(type a, type b, type coef) { return saturate(lincoef(a, b, coef)); }
template <typename type> constexpr inline type sqrval(type val) { return val * val; }
template <typename type, typename _type> constexpr inline type intdivceil(type val, _type div) { return (val - 1) / type(div) + 1; }
template <typename type, typename _type> constexpr inline type alignval(type val, _type mod) { return val / type(mod) * type(mod); }
template <typename type> constexpr inline type absval(type val) { return val >= type(0) ? val : -val; }
template <typename type> constexpr inline type minval(type val1, type val2) { return val1 < val2 ? val1 : val2; }
template <typename type> constexpr inline type maxval(type val1, type val2) { return val1 > val2 ? val1 : val2; }
template <typename type> constexpr inline type minval(type val1, type val2, type val3) { return minval(val1, minval(val2, val3)); }
template <typename type> constexpr inline type maxval(type val1, type val2, type val3) { return maxval(val1, maxval(val2, val3)); }

template <typename type> constexpr type pi();
template <> constexpr float32 pi<float32>() { return 3.14159265f; }
template <> constexpr float64 pi<float64>() { return 3.141592653589793238; }

inline uint32 asUint32(float32 val) { return *((uint32*)&val); }
inline uint64 asUint64(float64 val) { return *((uint64*)&val); }
inline float64 asFloat64(uint64 val) { return *((float64*)&val); }
inline float32 asFloat32(uint32 val) { return *((float32*)&val); }

template <typename type> inline type nan();
template <> inline float32 nan<float32>() { return asFloat32(0x7fc00000); }
template <> inline float64 nan<float64>() { return asFloat64(0x7ff8'0000'0000'0000ull); }

uint8 constexpr unormFloat32toUint8(float32 val) { return uint8(val * float32(uint8(-1))); }

#endif