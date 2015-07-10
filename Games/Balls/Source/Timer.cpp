#include <Windows.h>

#include "Timer.h"

float32 Timer::frequency = 0.0f;

void Timer::Initialize()
{
	uint64 frequencyU64 = 0;
	QueryPerformanceFrequency(PLARGE_INTEGER(&frequencyU64));
	frequency = float32(frequencyU64);
}

TimerRecord Timer::GetRecord()
{
	uint64 record = 0;
	QueryPerformanceCounter(PLARGE_INTEGER(&record));
	return TimerRecord(record);
}