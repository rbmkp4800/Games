#include <stdlib.h>
#include <time.h>

#include "Random.h"

void Random::Seed()
{
	srand(uint32(time(nullptr)));
}
void Random::Seed(uint32 seed)
{
	srand(seed);
}

bool Random::GetBool()
{
	return rand() % 2 ? true : false;
}
float32 Random::GetFloat32(float32 mod)
{
	return float32(rand()) / float32(RAND_MAX) * mod;
}
uint32 Random::GetUint32(uint32 mod)
{
	if (RAND_MAX >= mod)
		return rand() % mod;
	return (rand() * rand() * rand()) % mod;
}