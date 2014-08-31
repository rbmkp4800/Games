#pragma once

#ifndef _extypes_def_util_
#define _extypes_def_util_

#define elemcntof(arr)	(sizeof(arr) / sizeof(*arr))
#define xorswap(a1, a2)	((a1 ^= a2), (a2 ^= a1), (a1 ^= a2))

template <typename type>
inline type absval(type val) { return val > (type) 0 ? val : -val; }
template <typename type>
inline type minval(type val1, type val2) { return val1 < val2 ? val1 : val2; }
template <typename type>
inline type maxval(type val1, type val2) { return val1 > val2 ? val1 : val2; }
template <typename type>
inline type clamp(type val, type _min, type _max)
{
	if (val < _min) return _min;
	if (val > _max) return _max;
	return val;
}
template <typename type>
inline type saturate(type val) { return clamp(val, (type) 0, (type) 1); }
template <typename type>
inline type sqrval(type a) { return a * a; }
template <typename type, typename _type>
inline type intdivceil(type val, _type div) { return (type) ((val - 1) / div + 1); }

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

#ifndef min
#define min(val1, val2) ((val1) < (val2) ? (val1) : (val2))
#endif

#ifndef max
#define max(val1, val2) ((val1) > (val2) ? (val1) : (val2))
#endif

#endif