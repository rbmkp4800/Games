#include "BallsGame.h"

#include <extypes.matrix3x2.h>
#include <Random.h>
#include <Timer.h>

#include <memory.h>
#include <stdio.h>

using namespace Render2D;
using namespace BallsGame;

Device Game::device;

constexpr float32 playerBallDefaultOutputVerticalPos = 0.4f;
constexpr float32 playerBallControlsAccel = 2.0f;
constexpr float32 playerBallControlsDefaultCharge = 0.1f;
constexpr float32 cameraDeltaDecreaseExponentCoef = 0.2f;
constexpr float32 cameraDeltaCoef = 0.5f;
constexpr float32 gravityAccel = -0.4f;

bool Game::Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY)
{
	Timer::Initialize();

	if (!device.IsInitialized())
		device.Create();
	if (!swapChain.CreateForHWnd(&device, outputHWnd, outputSizeX, outputSizeY))
		return false;
	device.SetTarget(&swapChain);

	field.Initialize();
	background.Initialize();
	hell.Initialize();

	outputSize.set(outputSizeX, outputSizeY);
	aspect = float32(outputSizeX) / float32(outputSizeY);
	cameraDelta = 0.0f;
	globalPosition = 0.0f;
	maxGlobalPosition = 0.0f;

	memset(&controls, 0, sizeof(controls));

	return true;
}

void Game::ResizeOutput(uint32 x, uint32 y)
{
	if (device.IsInitialized())
	{
		device.ResetTarget();
		swapChain.Resize(&device, x, y);
		device.SetTarget(&swapChain);

		outputSize.set(x, y);
		aspect = float32(x) / float32(y);
	}
}

void Game::Update(float32 timeDelta)
{
	TimerRecord frameStartRecord = Timer::GetRecord();

	float32x2 acceleration(0.0f, gravityAccel);
	float32 playerBallCharge = 0.0f;

	if (controls.left)
		acceleration.x -= playerBallControlsAccel;
	if (controls.right)
		acceleration.x += playerBallControlsAccel;
	if (controls.up)
		acceleration.y += playerBallControlsAccel;
	if (controls.down)
		acceleration.y -= playerBallControlsAccel;
	if (controls.positiveCharge && !controls.negativeCharge)
		playerBallCharge += playerBallControlsDefaultCharge;
	if (controls.negativeCharge && !controls.positiveCharge)
		playerBallCharge -= playerBallControlsDefaultCharge;

	float32 posDelta = field.Update(timeDelta, acceleration, playerBallCharge);
	globalPosition += posDelta;
	if (globalPosition > maxGlobalPosition)
		maxGlobalPosition = globalPosition;

	/*float32 cameraDeltaDecreaseCoef = powf(cameraDeltaDecreaseExponentCoef, timeDelta);
	float32 cameraFullDelta = cameraDeltaCoef * playerBall.speed.y;
	cameraDelta = (cameraDelta - cameraFullDelta) * cameraDeltaDecreaseCoef + cameraFullDelta;*/

	/*device.SetTransform(translationScaleTranslation(-1.0f, -1.0f, 2.0f * a, 2.0f * aspect * a,
		0.0f, playerBallDefaultOutputVerticalPos / aspect), 1.0f / float32(outputSize.x));*/

	LocalMemoryBuffer<Device::defaultVertexBufferSize> buffer;
	Batch batch(&device, buffer.GetPointer(), buffer.GetSize());

	device.Clear(0x68C0EC_rgb);
	device.SetTransform(matrix3x2::identity());
	batch.PushGradientRect(rectf32(-1.0f, -1.0f, 1.0f, 0.3f), 0x297BA000_rgba, 0x297BA0_rgb, GradientType::Vertical);	//0x68C0EC_rgb
	batch.Flush();

	device.SetTransform(matrix3x2::scale(2.0f, 2.0f * aspect) * matrix3x2::translation(-0.5f, 0.0f), 1.0f / float32(outputSize.x));

	background.UpdateAndDraw(posDelta, cameraDelta, &batch);
	field.Draw(&batch);
	hell.UpdateAndDraw(posDelta, timeDelta, &batch);

	batch.Flush();

	device.SetTransform(matrix3x2::identity());
	batch.PushGradientEllipse(rectf32(-2.5f, -2.2f, 2.5f, 2.0f), Color(Colors::Black, 0), Colors::Black, 0.45f, 0.75f);
	batch.PushGradientRect(rectf32(-1.0f, -1.0f, 1.0f, -0.9f), Color(Colors::Black, 0), Color(Colors::Black, 128), GradientType::Vertical);
	batch.PushGradientRect(rectf32(-1.0f, 0.9f, 1.0f, 1.0f), Color(Colors::Black, 128), Color(Colors::Black, 0), GradientType::Vertical);
	batch.Flush();

	TimerRecord frameEndRecord = Timer::GetRecord();

	char infoString[256];
	sprintf(infoString, "Balls Game by �����4800. Work in progress LOL\nOutput size %dx%d. Frame time %.2f ms. Time delta %.2f ms.\nGlobal position %.2f/%.2f\n",
		outputSize.x, outputSize.y, Timer::ElapsedTime(frameStartRecord, frameEndRecord) * 1000.0f, timeDelta * 1000.0f, globalPosition, maxGlobalPosition);
	device.SetDirectTransform();
	batch.PushText(device.GetDefaultFont(), float32x2(5.0f, 5.0f), infoString);
	batch.Flush();

	swapChain.Present();
}

void Game::SetPlayerControlState(PlayerControl playerControl, bool state)
{
	switch (playerControl)
	{
	case PlayerControl::Left:
		controls.left = state;
		break;
	case PlayerControl::Right:
		controls.right = state;
		break;
	case PlayerControl::Up:
		controls.up = state;
		break;
	case PlayerControl::Down:
		controls.down = state;
		break;
	case PlayerControl::PositiveCharge:
		controls.positiveCharge = state;
		break;
	case PlayerControl::NegativeCharge:
		controls.negativeCharge = state;
		break;
	case PlayerControl::Jump:
		if (state) controls.jump = true;
		break;
	}
}