#include <stdlib.h>
#include <time.h>

#include "Random.h"

void Random::Seed()
{
	srand(time(nullptr));
}
void Random::Seed(uint32 seed)
{
	srand(seed);
}

bool Random::GetBool()
{
	return rand() % 2;
}
float Random::GetFloat(float mod)
{
	return float(rand()) / float(RAND_MAX) * mod;
}
uint32 Random::GetUint32(uint32 mod)
{
	if (RAND_MAX >= mod)
		return rand() % mod;
	return (rand() * rand() * rand()) % mod;
}