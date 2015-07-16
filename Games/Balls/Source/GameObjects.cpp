#include <extypes.vectors.math.h>

#include "BallsGame.h"

using namespace BallsGame;
using namespace Render2D;

//--------------------------StaticBall--------------------------//

constexpr float32 staticBallBounceSpeedReduceCoef = 0.5f;
constexpr float32 staticBallChargeAbsValue = 1.0f;
constexpr uint32 staticBallNeutralColor = Colors::Gray;
constexpr uint32 staticBallPositiveColor = Colors::SkyBlue;
constexpr uint32 staticBallNegativeColor = Colors::LightSalmon;
constexpr float32 staticBallDistanceForceOpac0 = 0.8f;
constexpr float32 staticBallDistanceForceOpac1 = 0.3f;

float32 StaticBall::GetPlayerBallCollisionDistance(PlayerBall& playerBall, const float32x2& translation, float32 translationLength)
{
	float32x2 distanceVector = position - playerBall.position;
	float32 distance = length(distanceVector);
	float32 minDistance = playerBall.radius + radius;
	if (distance > minDistance + translationLength)
		return nan<float32>();
	float32 distTransAngleCos = dot(distanceVector, translation) / (distance * translationLength);
	float32 discriminant = sqrval(distTransAngleCos) + sqrval(minDistance / distance) - 1.0f;
	if (discriminant < 0.0f)
		return nan<float32>();
	float32 collisionDistance = distance * (distTransAngleCos - sqrtf(discriminant));
	if (collisionDistance < 0.0f)
		return nan<float32>();
	return collisionDistance;
}
void StaticBall::CollideWithPlayerBall(float32 collisionDistance, PlayerBall& playerBall, float32x2& translation, float32 translationLength)
{
	bool b = length(playerBall.position - position) <= playerBall.radius + radius;

	float32x2 fallVector = translation * (collisionDistance / translationLength);
	float32x2 normalVector = fallVector - (position - playerBall.position);
	float32x2 reflectionVector = (translation - proj(translation, normalVector) * 2.0f) / translationLength;
	translation = reflectionVector * (translationLength - collisionDistance);
	playerBall.position += fallVector;
	playerBall.speed = reflectionVector * length(playerBall.speed) * staticBallBounceSpeedReduceCoef;
	float32 l = length(playerBall.position - position);
	if (length(playerBall.position - position) < playerBall.radius + radius)
	{
		int a = 0;
		a++;
	}
}

float32x2 StaticBall::GetForceAppliedToObject(float32x2 objectPosition, float32 objectCharge)
{
	float32x2 distanceVector = position - objectPosition;
	float32 distance = length(distanceVector);
	return distanceVector * (charge == Charge::Positive ? staticBallChargeAbsValue : -staticBallChargeAbsValue)
		* objectCharge / (distance * distance * distance);
}

void StaticBall::DrawForce(Render2D::Batch* batch, const PlayerBall& playerBall, float32 playerBallCharge)
{
	float32 thisBallCharge = 0.0f;
	switch (charge)
	{
	case Charge::Positive: thisBallCharge = 1.0f; break;
	case Charge::Negative: thisBallCharge = -1.0f; break;
	default: return;
	}

	float32 playerBallDistance = length(position - playerBall.position);
	float32 opacity = lincoefsatur(staticBallDistanceForceOpac0, staticBallDistanceForceOpac1, playerBallDistance);
	if (opacity >= opacityThreshold)
	{
		uint32 color = thisBallCharge * playerBallCharge < 0.0f ? staticBallNegativeColor : staticBallPositiveColor;
		Color colorCenter(color, unormFloat32toUint8(opacity)), colorSide(color, 0);
		float32 width = lerp(0.01f, 0.08f, lincoefsatur(0.6f, 0.1f, playerBallDistance));
		batch->PushLineAligned(position, playerBall.position, width, colorSide, colorCenter, GradientType::Horizontal);
		batch->PushLineAligned(position, playerBall.position, -width, colorSide, colorCenter, GradientType::Horizontal);
	}
}

void StaticBall::Draw(Render2D::Batch* batch)
{
	Color color = 0;
	switch (charge)
	{
	case Charge::Positive: color = staticBallPositiveColor; break;
	case Charge::Negative: color = staticBallNegativeColor; break;
	default: color = staticBallNeutralColor; break;
	}
	batch->PushCircleAA(position, radius * 0.8f, color);
	batch->PushCircleAA(position, radius, Colors::White, 0.89f);
}