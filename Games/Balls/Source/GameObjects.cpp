#include "BallsGame.h"

#include <extypes.vectors.math.h>

using namespace BallsGame;

//--------------------------StaticBall--------------------------//

Render2D::Device Game::device;

void StaticBall::CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& playerBallTranslation)
{

}

float32x2 StaticBall::GetForceAppliedToPlayerBall(const PlayerBall& playerBall)
{
	float32x2 distanceVector = position - playerBall.position;
	float distance = length(distanceVector);
	return float32x2(0.0f, 0.0f);
}