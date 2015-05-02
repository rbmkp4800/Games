#include "BallsGame.h"

using namespace BallsGame;

void Field::Clear()
{
	staticBallsCount = 0;
}

void Field::InteractPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2 playerBallTranslation)
{
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBalls[i].CollideWithPlayerBall(timeDelta, playerBall, playerBallTranslation);
	playerBall.position += playerBallTranslation;
	playerBall.acceleration.set(0.0f, 0.0f);
	for (uint32 i = 0; i < staticBallsCount; i++)
		playerBall.acceleration += staticBalls[i].GetForceAppliedToPlayerBall(playerBall);
}

void Field::UpdateAndDraw(float posDelta, Render2D::Batch* batch)
{
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBalls[i].Move(posDelta);
}