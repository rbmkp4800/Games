#include <stdlib.h>
#include <time.h>

#include "Timer.h"

#include "BallsGame.h"
#include "BallsGame.Interface.h"

using namespace Render2D;
using namespace BallsGame;

Game::Message Game::messages[Game::messagesLimit];
uint32 Game::firstMessage = 0, Game::lastMessage = 0;
bool Game::initialized = false;

float timeDelta = 0.0f;
PlayerBall playerBall;

void Game::Run(void* output, uint32 outputSizeX, uint32 outputSizeY)
{
	float outputAspect = float(outputSizeX) / float(outputSizeY);
	float fieldHeight = FIELD_WIDTH / outputAspect;
	float cameraTargetOutputDisplacement = fieldHeight * CAMERA_TARGET_OUTPUT_DISPLACEMENT_POSCOEF;
	matrix3x2 cameraBaseScaleMatrix = matrix3x2::scale((float) outputSizeX / FIELD_WIDTH);
	matrix3x2 cameraBaseMatrix = cameraBaseScaleMatrix * matrix3x2::translation(FIELD_WIDTH / 2.0f - 0.5f, 0.0f);

	float cameraTarget = 0.0f;
	int32 cameraTargetBase = 0;
	float backTextureDisplacement = 0.0f;
	int32 backTextureTopBlockIdx = 0;

	Render::Init();
	Render::SetSamplerMode(SamplerMode::Linear_TextAddressWrap);
	SwapChain swapChain;
	RenderTarget shadowsRenderTarget;
	swapChain.CreateForHWnd(output, outputSizeX, outputSizeY);
	shadowsRenderTarget.Create(SHADOWS_BITMAP_WIDTH, (uint32) (SHADOWS_BITMAP_WIDTH * fieldHeight / SIDEWALL_INNER_PERSPECCOEF));
	Textures::Load();

	GameObjectsController gameObjectsController;

	Game::InitMessagesStack();

	for (;;)
	{
		struct { bool up, down, left, right, positiveCharge, negativeCharge, slow; } controlsState = { 0 };

		srand((uint32) time(nullptr));

		playerBall.radius = 0.05f;
		playerBall.position.set(0.5f, 0.0f);
		playerBall.speed.set(0.0f, -1.0f);
		playerBall.acceleration.set(0.0f, 0.0f);

		Timer timer;
		float timeCoef = 1.0f;

		double currentDistance = 0.0, totalDistance = 0.0, lastChargedBallCreationDistance = 0.0;
		float lastCharge = 1.0f;

		for (;;)
		{
			bool restart = false;
			for (;;)
			{
				Message message = Game::PopMessage();
				if (message.type == MessageType::None)
					break;
				switch (message.type)
				{
				case MessageType::ChangeControlState:
					switch ((Control) message.a)
					{
					case Control::Up:
						controlsState.up = (bool) message.b;
						break;
					case Control::Down:
						controlsState.down = (bool) message.b;
						break;
					case Control::Left:
						controlsState.left = (bool) message.b;
						break;
					case Control::Right:
						controlsState.right = (bool) message.b;
						break;
					case Control::PositiveCharge:
						controlsState.positiveCharge = (bool) message.b;
						break;
					case Control::NegativeCharge:
						controlsState.negativeCharge = (bool) message.b;
						break;
					case Control::Slow:
						controlsState.slow = (bool) message.b;
						break;
					}
					break;

				case MessageType::ResizeOutput:
					break;

				case MessageType::Restart:
					restart = true;
					break;
				}
			}
			if (restart)
				break;

			if (controlsState.slow)
			{
				if (timeCoef - 0.04f < 0.3f)
					timeCoef = 0.3f;
				else
					timeCoef -= 0.04f;
			}
			else
			{
				if (timeCoef + 0.04f > 1.0f)
					timeCoef = 1.0f;
				else
					timeCoef += 0.04f;
			}
			timeDelta = minval(timer.GetTimeAndReset(), 0.03f) * timeCoef;

			if (controlsState.left)
				playerBall.acceleration.x -= ACCEL_PLAYERBALL_CONTROL;
			if (controlsState.right)
				playerBall.acceleration.x += ACCEL_PLAYERBALL_CONTROL;
			if (controlsState.up)
				playerBall.acceleration.y -= ACCEL_PLAYERBALL_CONTROL;
			if (controlsState.down)
				playerBall.acceleration.y += ACCEL_PLAYERBALL_CONTROL;
			if (controlsState.negativeCharge)
				playerBall.charge = -1.0f;
			if (controlsState.positiveCharge)
				playerBall.charge = 1.0f;
			if (!(controlsState.negativeCharge ^ controlsState.positiveCharge))
				playerBall.charge = 0.0f;
			playerBall.acceleration.y += ACCEL_GRAVITY;
			playerBall.acceleration -= ACCEL_FRICTION;

			playerBall.UpdateTranslation(timeDelta);
			gameObjectsController.Dispatch(GameObject::Message::UpdateForces);
			gameObjectsController.Dispatch(GameObject::Message::UpdateCollisions);
			playerBall.position += playerBall.translation;
			playerBall.CheckWallsCollisions();

			float cameraTargetTranslation = (playerBall.position.y - cameraTarget - playerBall.speed.y *
				((CAMERA_EXPON_DIST_DERCEASE_COEF - 1.0f) * CAMERA_EXPON_DIST_DERCEASE_DELTA + timeDelta)) *
				(1.0f - powf(1.0f - 1.0f / CAMERA_EXPON_DIST_DERCEASE_COEF, timeDelta / CAMERA_EXPON_DIST_DERCEASE_DELTA)) + playerBall.speed.y * timeDelta;
			cameraTarget += cameraTargetTranslation;
			backTextureDisplacement -= cameraTargetTranslation;

			/*if (cameraTarget > 5.0f)
			{
			cameraTargetBase += 5;
			goc.MoveAll(-5.0f);
			playerBall.position.y -= 5.0f;
			cameraTarget -= 5.0f;
			}
			if (cameraTarget < -5.0f)
			{
			cameraTargetBase -= 5;
			goc.MoveAll(5.0f);
			playerBall.position.y += 5.0f;
			cameraTarget += 5.0f;
			}*/

			totalDistance = (double) playerBall.position.y;
			if (lastChargedBallCreationDistance - totalDistance > 0.35)
			{
				switch (rand() % 3)
				{
				case 0:
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.42f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y + 1.4f), 0.04f, lastCharge);
					break;

				case 1:
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.42f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.4f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.38f), 0.04f, lastCharge);
					break;

				case 2:
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.42f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.4f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.36f), 0.04f, lastCharge);
					gameObjectsController.CreateObject<ChargedStaticBall>(float32x2((rand() % 100) / 100.0f, playerBall.position.y - 1.38f), 0.04f, lastCharge);
					break;
				}
				lastChargedBallCreationDistance = totalDistance;
				lastCharge = -lastCharge;
			}

			Render::SetTarget(&shadowsRenderTarget);
			Render::Clear(colors::transparent);
			Render::SetTransform(matrix3x2::scale(SHADOWS_BITMAP_WIDTH) * matrix3x2::translation(0.0f, -cameraTarget +
				cameraTargetOutputDisplacement + fieldHeight / 2.0f * (1.0f / SIDEWALL_INNER_PERSPECCOEF - 1.0f)));
			gameObjectsController.Dispatch(GameObject::Message::DrawShadows);
			Render::DrawEllipse(playerBall.position, playerBall.radius, colors::black);

			Render::SetTarget(&swapChain);
			Render::Clear(colors::cornflowerBlue);
			Render::SetTransform(cameraBaseScaleMatrix);
			while (backTextureDisplacement > fieldHeight / 2.0f * (1.0f - 1.0f / SIDEWALL_INNER_PERSPECCOEF))
			{
				backTextureTopBlockIdx++;
				backTextureDisplacement -= BACKGROUND_BLOCK_SIZE;
			}

			float32x2 vb[4];
			float currentDisplacement; int32 i;
			for (i = 0, currentDisplacement = backTextureDisplacement - fieldHeight / 2.0f;
				currentDisplacement < fieldHeight / 2.0f / SIDEWALL_INNER_PERSPECCOEF; i++)
			{
				float outer0 = currentDisplacement * SIDEWALL_OUTER_PERSPECCOEF + fieldHeight / 2.0f;
				float inner0 = currentDisplacement * SIDEWALL_INNER_PERSPECCOEF + fieldHeight / 2.0f;
				float outer1 = currentDisplacement * SIDEWALL_OUTER_PERSPECCOEF + BACKGROUND_BLOCK_SIZE * SIDEWALL_OUTER_PERSPECCOEF + fieldHeight / 2.0f;
				float inner1 = currentDisplacement * SIDEWALL_INNER_PERSPECCOEF + BACKGROUND_BLOCK_SIZE * SIDEWALL_INNER_PERSPECCOEF + fieldHeight / 2.0f;
				coloru32 color = (backTextureTopBlockIdx + i) % 2 ? colors::darkBlue : colors::lightGray;
				Render::DrawFillRect(&rectf32(SIDEWALL_WIDTH, inner0, SIDEWALL_WIDTH + BACKWALL_WIDTH, inner1), color);
				vb[0].x = vb[3].x = 0.0f;
				vb[1].x = vb[2].x = SIDEWALL_WIDTH;
				vb[0].y = outer1; vb[1].y = inner1;
				vb[3].y = outer0; vb[2].y = inner0;
				Render::DrawQuadrangle(vb, color);
				vb[0].x = vb[3].x = BACKWALL_WIDTH + SIDEWALL_WIDTH;
				vb[1].x = vb[2].x = BACKWALL_WIDTH + SIDEWALL_WIDTH * 2.0f;
				vb[0].y = inner1; vb[1].y = outer1;
				vb[3].y = inner0; vb[2].y = outer0;
				Render::DrawQuadrangle(vb, color);
				currentDisplacement += BACKGROUND_BLOCK_SIZE;
			}

			Render::DrawBitmap(&Textures::backgroundMask,
				&rectf32(SIDEWALL_WIDTH, 0.0f, SIDEWALL_WIDTH + BACKWALL_WIDTH, fieldHeight),
				&rectf32(0.0f, 0.0f, 0.8f, 1.0f), 0.3f);
			Render::DrawBitmap(&Textures::backgroundMask, &rectf32(0.0f, 0.0f, SIDEWALL_WIDTH, fieldHeight),
				&rectf32(1.0f, 0.0f, 0.8f, 1.0f), 0.5f);
			Render::DrawBitmap(&Textures::backgroundMask, &rectf32(SIDEWALL_WIDTH + BACKWALL_WIDTH, 0.0f, FIELD_WIDTH, fieldHeight),
				&rectf32(0.8f, 0.0f, 1.0f, 1.0f), 0.5f);
			Render::DrawBitmap(&shadowsRenderTarget, &rectf32(SIDEWALL_WIDTH, 0.0f, SIDEWALL_WIDTH + BACKWALL_WIDTH, fieldHeight), nullptr, 0.4f);

			Render::SetTransform(cameraBaseMatrix * matrix3x2::translation(0.0f, -cameraTarget + cameraTargetOutputDisplacement));
			gameObjectsController.Dispatch(GameObject::Message::Draw);

			Render::DrawEllipse(playerBall.position, playerBall.radius - 0.001f, colors::white);
			Render::DrawBitmap(&Textures::ballMask, &rectf32(playerBall.position.x - playerBall.radius, playerBall.position.y - playerBall.radius,
				playerBall.position.x + playerBall.radius, playerBall.position.y + playerBall.radius));
			Render::DrawEllipse(playerBall.position, playerBall.radius, colors::black, 0.002f);

			Render::Flush();
			swapChain.Present();
		}
	}
}