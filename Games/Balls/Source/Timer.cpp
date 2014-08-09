#include <Windows.h>

#include "Timer.h"

float Timer::frequency = 0.0f;

void Timer::InitTimerBase()
{
	if (frequency == 0.0f)
	{
		uint64 frequencyU64 = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*) &frequencyU64);
		frequency = (float) frequencyU64;
	}
}

void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*) &resetTick);
}
float Timer::GetTime()
{
	if (resetTick)
	{
		uint64 currentTick;
		QueryPerformanceCounter((LARGE_INTEGER*) &currentTick);
		return (currentTick - resetTick) / frequency;
	}
	return 0.0f;
}
float Timer::GetTimeAndReset()
{
	if (resetTick)
	{
		uint64 currentTick;
		QueryPerformanceCounter((LARGE_INTEGER*) &currentTick);
		float result = (currentTick - resetTick) / frequency;
		resetTick = currentTick;
		return result;
	}
	QueryPerformanceCounter((LARGE_INTEGER*) &resetTick);
	return 0.0f;
}