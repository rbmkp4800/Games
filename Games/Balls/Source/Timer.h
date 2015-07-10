#pragma once

#include <extypes.h>

using TimerRecord = uint64;

class Timer abstract final
{
private:
	static float32 frequency;

public:
	static void Initialize();
	static TimerRecord GetRecord();
	static inline float32 ElapsedTime(TimerRecord record1, TimerRecord record2)
	{
		return float32(record2 - record1) / frequency;
	}
};