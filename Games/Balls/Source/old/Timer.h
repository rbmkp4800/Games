#include <extypes.h>

class Timer
{
private:
	static float frequency;
	static void InitTimerBase();

	uint64 resetTick;

public:
	inline Timer() : resetTick(0) { InitTimerBase(); }
	inline void Discard() { resetTick = 0; }
	void Reset();
	float GetTime();
	float GetTimeAndReset();
};