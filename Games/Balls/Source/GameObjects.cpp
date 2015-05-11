#include <extypes.vectors.math.h>

#include "BallsGame.h"

using namespace BallsGame;

//--------------------------StaticBall--------------------------//

static const float staticBallBounceSpeedReduceCoef = 1.0f;

void StaticBall::CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& translation)
{
	float32x2 distanceVector = position - playerBall.position;
	if (!translation.any())
		return;
	float distance = length(distanceVector);
	float translationLength = length(translation);
	float minDistance = playerBall.radius + radius;
	if (distance > minDistance + translationLength)
		return;
	if (distance < minDistance)
	{
		distanceVector *= minDistance / distance;
		playerBall.position = position - distanceVector;
		distance = minDistance;
	}

	float distTransAngleCos = dot(distanceVector, translation) / (distance * translationLength);
	float discriminant = sqrval(distTransAngleCos) + sqrval(minDistance / distance) - 1.0f;
	if (discriminant < 0.0f)
		return;
	float fallingDistance = distance * (distTransAngleCos - sqrtf(discriminant));
	if (fallingDistance <= 0.0f || fallingDistance > translationLength)
		return;

	float32x2 fallVector = (translation / translationLength) * fallingDistance;
	float32x2 normalVector = fallVector - distanceVector;
	float32x2 reflectionVector = (fallVector - normalVector * dot(normalVector, fallVector) / dot(normalVector, normalVector) * 2.0f) / fallingDistance;
	translation = reflectionVector * (translationLength - fallingDistance);
	playerBall.position += fallVector;
	playerBall.speed = reflectionVector * length(playerBall.speed) * staticBallBounceSpeedReduceCoef;
}

float32x2 StaticBall::GetForceAppliedToPlayerBall(const PlayerBall& playerBall)
{
	float32x2 distanceVector = position - playerBall.position;
	float distance = length(distanceVector);
	return float32x2(0.0f, 0.0f);
}

void StaticBall::Draw(Render2D::Batch* batch)
{
	using namespace Render2D;
	//batch->PushCircleAA(position, radius * 0.8, colors::lightSalmon);
	batch->PushCircleAA(position, radius * 0.8f, charge < 0.0f ? colors::skyBlue : colors::lightSalmon);
	batch->PushCircleAA(position, radius, colors::white, 0.89f);
}