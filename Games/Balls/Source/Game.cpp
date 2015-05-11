#include "BallsGame.h"

#include <extypes.matrix3x2.h>
#include <memory.h>

using namespace Render2D;
using namespace BallsGame;

Device Game::device;

static const float playerBallDefaultRadius = 0.05f;
static const float playerBallDefaultOutputVerticalPos = 0.4f;
static const float playerBallUserControlsAccel = 2.0f;
static const float cameraDeltaDecreaseExponentCoef = 0.1f;
static const float cameraDeltaCoef = 0.3f;
static const float gravityAccel = 0.4f;

bool Game::Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY)
{
	if (!device.IsInitialized())
		device.Create();
	if (!swapChain.CreateForHWnd(&device, outputHWnd, outputSizeX, outputSizeY))
		return false;
	device.SetTarget(&swapChain);

	field.Clear();
	background.Generate();

	outputSize.set(outputSizeX, outputSizeY);
	aspect = float(outputSizeX) / float(outputSizeY);
	cameraDelta = 0.0f;
	globalPosition = 0.0;

	memset(&controls, 0, sizeof(controls));
	memset(&playerBall, 0, sizeof(playerBall));
	playerBall.position.x = 0.5f;
	playerBall.speed.y = 1.0f;
	playerBall.radius = playerBallDefaultRadius;

	return true;
}

inline matrix3x2 translationScaleTranslation(float xtrans1, float ytrans1, float xscale, float yscale, float xtrans2, float ytrans2)
{
	matrix3x2 matrix;
	matrix.data[0][0] = xscale;
	matrix.data[0][1] = 0.0f;
	matrix.data[1][0] = 0.0f;
	matrix.data[1][1] = yscale;
	matrix.data[2][0] = xtrans1 + xtrans2 * xscale;
	matrix.data[2][1] = ytrans1 + ytrans2 * yscale;
	return matrix;
}

void Game::Update(float timeDelta)
{
	float32x2 acceleration = field.GetForceAppliedToPlayerBall(playerBall);
	acceleration.y -= gravityAccel;

	if (controls.left)
		acceleration.x -= playerBallUserControlsAccel;
	if (controls.right)
		acceleration.x += playerBallUserControlsAccel;
	if (controls.up)
		acceleration.y += playerBallUserControlsAccel;
	if (controls.down)
		acceleration.y -= playerBallUserControlsAccel;

	float32x2 translation = (playerBall.speed + acceleration * timeDelta / 2.0f) * timeDelta;
	playerBall.speed += acceleration * timeDelta;
	field.CollideWithPlayerBall(timeDelta, playerBall, translation);
	playerBall.position += translation;
	float posDelta = playerBall.position.y;
	playerBall.position.y = 0.0f;
	globalPosition += float64(posDelta);

	if (playerBall.position.x < playerBall.radius)
	{
		if (playerBall.speed.x < 0.0f)
			playerBall.speed.x = -playerBall.speed.x;
		playerBall.position.x = playerBall.radius + playerBall.radius - playerBall.position.x;
	}
	if (playerBall.position.x > 1.0f - playerBall.radius)
	{
		if (playerBall.speed.x > 0.0f)
			playerBall.speed.x = -playerBall.speed.x;
		playerBall.position.x = 2.0f - playerBall.radius - playerBall.radius - playerBall.position.x;
	}

	float cameraDeltaDecreaseCoef = powf(cameraDeltaDecreaseExponentCoef, timeDelta);
	float cameraFullDelta = cameraDeltaCoef * playerBall.speed.y;
	cameraDelta = (cameraDelta - cameraFullDelta) * cameraDeltaDecreaseCoef + cameraFullDelta;
	device.SetTransform(translationScaleTranslation(-1.0f, -1.0f, 2.0f, 2.0f * aspect,
		0.0f, playerBallDefaultOutputVerticalPos / aspect + cameraDelta), 1.0f / float(outputSize.x));

	LocalMemoryBuffer<1024> buffer;
	Batch batch(&device, buffer.GetPointer(), buffer.GetSize());

	background.UpdateAndDraw(-posDelta, &batch);
	field.UpdateAndDraw(-posDelta, &batch);

	batch.PushCircleAA(playerBall.position, playerBall.radius * 0.85f, colors::white);
	batch.PushCircleAA(playerBall.position, playerBall.radius, colors::white, 0.92f);
	batch.Flush();

	device.SetTransform(matrix3x2::identity());
	batch.PushGradientEllipse(rectf32(-2.0f, -2.0f, 2.0f, 2.0f),
		colors::transparent, coloru32(colors::black, 0.7f), 0.45f, 0.8f);
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
	}
}