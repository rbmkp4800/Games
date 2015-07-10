#pragma once

#include <extypes.h>

class Random abstract final
{
public:
	static void Seed();
	static void Seed(uint32 seed);

	static bool GetBool();
	static float32 GetFloat32(float32 mod);
	static uint32 GetUint32(uint32 mod);
};