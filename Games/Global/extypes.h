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

using uint = unsigned int;
using sing = signed int;

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
template <typename type, uint size>
uint elemcntof(type(&)[size]) { return size; }
template <typename type>
inline type absval(type val) { return val >= type(0) ? val : -val; }
template <typename type1, typename type2>
inline type1 minval(type1 val1, type2 val2) { return val1 < val2 ? val1 : val2; }
template <typename type1, typename type2>
inline type1 maxval(type1 val1, type2 val2) { return val1 > val2 ? val1 : val2; }
template <typename type>
inline type clamp(type val, type _min, type _max)
{
	if (val < _min) return _min;
	if (val > _max) return _max;
	return val;
}
template <typename type>
inline type saturate(type val) { return clamp(val, type(0), type(1)); }
template <typename type>
inline type sqrval(type a) { return a * a; }
template <typename type, typename _type>
inline type intdivceil(type val, _type div) { return (type) ((val - 1) / div + 1); }
template <typename type, typename _type>
inline type alignval(type val, _type mod) { return val / mod * mod; }

#define elemcntof_constexpr(arr) (sizeof(arr) / sizeof(*arr))

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

#define PI_def 3.141592654f

#endif