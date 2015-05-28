#include <extypes.h>

class Random abstract
{
public:
	static void Seed();
	static void Seed(uint32 seed);

	static bool GetBool();
	static float GetFloat(float mod);
	static uint32 GetUint32(uint32 mod);
};