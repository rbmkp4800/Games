#include "BallsGame.h"

#include <extypes.matrix3x2.h>
#include <memory.h>

using namespace Render2D;
using namespace BallsGame;

Device Game::device;

static const float playerBallDefaultRadius = 0.06f;
static const float playerBallDefaultOutputVerticalPos = 0.4f;
static const float playerBallControlsAccel = 2.0f;
static const float playerBallControlsDefaultCharge = 0.1f;
static const float cameraDeltaDecreaseExponentCoef = 0.1f;
static const float cameraDeltaCoef = 0.2f;
static const float gravityAccel = -0.4f;

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
	playerBall.position.set(0.5f, 0.0f);
	playerBall.speed.set(0.0f, 1.0f);
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
	float32x2 acceleration(0.0f, gravityAccel);
	float playerBallCharge = 0;

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

	acceleration += field.GetForceAppliedToPlayerBall(playerBall, playerBallCharge);

	float32x2 translation = (playerBall.speed + acceleration * timeDelta / 2.0f) * timeDelta;
	playerBall.speed += acceleration * timeDelta;
	field.CollideWithPlayerBall(timeDelta, playerBall, translation);
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

	LocalMemoryBuffer<Device::defaultVertexBufferSize> buffer;
	Batch batch(&device, buffer.GetPointer(), buffer.GetSize());

	device.SetTransform(translationScaleTranslation(-1.0f, -1.0f, 2.0f, 2.0f * aspect,
		0.0f, playerBallDefaultOutputVerticalPos / aspect), 1.0f / float(outputSize.x));
	background.UpdateAndDraw(-posDelta, &batch);
	batch.Flush();

	device.SetTransform(translationScaleTranslation(-1.0f, -1.0f, 2.0f, 2.0f * aspect,
		0.0f, playerBallDefaultOutputVerticalPos / aspect + cameraDelta), 1.0f / float(outputSize.x));
	field.UpdateAndDraw(-posDelta, &batch, playerBall, playerBallCharge);

	batch.PushCircleAA(playerBall.position, playerBall.radius * 0.85f, colors::white);
	batch.PushCircleAA(playerBall.position, playerBall.radius, colors::white, 0.92f);
	//batch.PushCircleAA(playerBall.position, playerBall.radius, colors::white);
	batch.Flush();

	device.SetTransform(matrix3x2::identity());
	batch.PushGradientEllipse(rectf32(-2.5f, -2.2f, 2.5f, 2.0f),
		colors::transparent, coloru32(colors::black), 0.45f, 0.8f);
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
	}
}