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
template <typename type, uintptr size>
uintptr elemcntof(type(&)[size]) { return size; }

template <typename type>
inline type absval(type val) { return val >= type(0) ? val : -val; }

template <typename type>
inline type minval(type val1, type val2) { return val1 < val2 ? val1 : val2; }

template <typename type>
inline type maxval(type val1, type val2) { return val1 > val2 ? val1 : val2; }

template <typename type>
inline type minval(type val1, type val2, type val3) { return minval(val1, minval(val2, val3)); }

template <typename type>
inline type maxval(type val1, type val2, type val3) { return maxval(val1, maxval(val2, val3)); }

template <typename type>
inline type clamp(type val, type _min, type _max)
{
	if (val < _min) return _min;
	if (val > _max) return _max;
	return val;
}
template <typename type>
inline type saturate(type val) { return clamp(val, type(0), type(1)); }
template <typename vectorType, typename coefType>
inline vectorType lerp(const vectorType& x, const vectorType& y, coefType coef) { return x + coef * (y - x); }
template <typename type>
inline type lincoef(type a, type b, type coef) { return (a - coef) / (a - b); }
template <typename type>
inline type lincoefsatur(type a, type b, type coef) { return saturate(lincoef(a, b, coef)); }
template <typename type>
inline type sqrval(type val) { return val * val; }
template <typename type>
inline type intdivceil(type val, type div) { return (val - 1) / div + 1; }
template <typename type>
inline type alignval(type val, type mod) { return val / mod * mod; }

#define elemcntof_constexpr(arr) (sizeof(arr) / sizeof(*arr))
#define minval_constexpr(val1, val2) ((val1) < (val2) ? (val1) : (val2))
#define maxval_constexpr(val1, val2) ((val1) > (val2) ? (val1) : (val2))
#define intdivceil_constexpr(val, div) (((val) - 1) / (div) + 1)

#define PI_def 3.141592654f

#endif