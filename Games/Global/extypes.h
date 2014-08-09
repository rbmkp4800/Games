#pragma once

#ifndef _extypes_def_extypes_
#define _extypes_def_extypes_

typedef char int8;
typedef short int int16;
typedef int int32;
typedef long long int int64;
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;
typedef float float32;
typedef double float64;

#ifdef _WIN64
typedef uint64 uintptr;
#else
typedef __w64 uint32 uintptr;
#endif

typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;
typedef int8 s8;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;
typedef float32 f32;
typedef float64 f64;

#endif