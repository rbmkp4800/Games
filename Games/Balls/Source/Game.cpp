#include "BallsGame.h"

using namespace Render2D;
using namespace BallsGame;

static const float playerBallDefaultRadius = 0.05f;
static const float playerBallDefaultOutputVerticalPos = 0.7f;

bool Game::Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY)
{
	if (!device.IsInitialized())
		device.Create();
	if (!swapChain.CreateForHWnd(&device, outputHWnd, outputSizeX, outputSizeY))
		return false;
	device.SetTarget(&swapChain);

	playerBall.position.set(0.0f, 0.0f);
	playerBall.speed.set(0.0f, 0.0f);
	playerBall.acceleration.set(0.0f, 0.0f);
	playerBall.radius = playerBallDefaultRadius;
	playerBall.charge = 0.0f;

	field.Clear();
	background.Generate();

	return true;
}

void Game::Update(float timeDelta)
{
	/*float32x2 playerBallTranslation = (playerBall.speed + playerBall.acceleration * timeDelta / 2.0f) * timeDelta;
	playerBall.speed += playerBall.acceleration * timeDelta;

	field.InteractPlayerBall(timeDelta, playerBall, playerBallTranslation);
	float posDelta = -playerBall.position.y;
	playerBall.position.y = 0.0f;
	background.UpdateAndDraw(posDelta, &device);
	field.UpdateAndDraw(posDelta, &device);*/

	device.Clear(colors::darkMagenta);
	VertexColor vb [] =
	{
		{ float32x2(-1.0f, -1.0f), coloru32(colors::red) },
		{ float32x2(0.0f, -1.0f), coloru32(colors::green) },
		{ float32x2(0.0f, 0.0f), coloru32(colors::blue) },
		{ float32x2(-1.0f, 0.0f), coloru32(colors::yellow) },
	};
	device.DrawColored(vb, 4, IndexationMode::Quad);

	swapChain.Present();
}