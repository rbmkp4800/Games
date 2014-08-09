#include <Windows.h>

#include "Bomberman.h"

using namespace Bomberman;

static uint64 systemTick;
static float currentSpeed, targetSpeed, speedChangePitch;

void GameTimer::ChangeSpeed(float target, float pitch)
{
	targetSpeed = target;
	speedChangePitch = pitch;
}

void GameTimer::Init()
{
	systemTick = 0;
	currentSpeed = 1.0f;
	speedChangePitch = 0.0f;
}

uint16 GameTimer::Update()
{
	if (!systemTick) systemTick = GetTickCount64();
	uint64 currentSystemTick = GetTickCount64();
	uint16 delta = (uint16)round((float)(currentSystemTick - systemTick) * currentSpeed);
	systemTick = currentSystemTick;

	if (speedChangePitch != 0.0f)
	{
		if (targetSpeed > currentSpeed)
		{
			currentSpeed += speedChangePitch;
			if (targetSpeed <= currentSpeed)
			{
				speedChangePitch = 0.0f;
				currentSpeed = targetSpeed;
			}
		}
		if (targetSpeed < currentSpeed)
		{
			currentSpeed -= speedChangePitch;
			if (targetSpeed >= currentSpeed)
			{
				speedChangePitch = 0.0f;
				currentSpeed = targetSpeed;
			}
		}
		if (currentSpeed == targetSpeed)
			speedChangePitch = 0.0f;
	}

	return delta ? delta : 1;
}