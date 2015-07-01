#include <extypes.vectors.math.h>

#include "BallsGame.h"

using namespace BallsGame;

//--------------------------StaticBall--------------------------//

static const float staticBallBounceSpeedReduceCoef = 0.5f;
static const float staticBallChargeAbsValue = 1.0f;
static const uint32 staticBallNeutralColor = Render2D::colors::gray;
static const uint32 staticBallPositiveColor = Render2D::colors::skyBlue;
static const uint32 staticBallNegativeColor = Render2D::colors::lightSalmon;
static const float staticBallDistanceForceOpac0 = 0.8f;
static const float staticBallDistanceForceOpac1 = 0.1f;

float StaticBall::GetPlayerBallCollisionDistance(PlayerBall& playerBall, const float32x2& translation, float translationLength)
{
#pragma message("make constexpr nan")
#define nan 1000.0f

	float32x2 distanceVector = position - playerBall.position;
	float distance = length(distanceVector);
	float minDistance = playerBall.radius + radius;
	if (distance > minDistance + translationLength)
		return nan;
	float distTransAngleCos = dot(distanceVector, translation) / (distance * translationLength);
	float discriminant = sqrval(distTransAngleCos) + sqrval(minDistance / distance) - 1.0f;
	if (discriminant < 0.0f)
		return nan;
	float collisionDistance = distance * (distTransAngleCos - sqrtf(discriminant));
	if (collisionDistance < 0.0f)
		return nan;
	return collisionDistance;
#undef nan
}
void StaticBall::CollideWithPlayerBall(float collisionDistance, PlayerBall& playerBall, float32x2& translation, float translationLength)
{
	float32x2 fallVector = translation * (collisionDistance / translationLength);
	float32x2 normalVector = fallVector - (position - playerBall.position);
	float32x2 reflectionVector = (translation - proj(translation, normalVector) * 2.0f) / translationLength;
	translation = reflectionVector * (translationLength - collisionDistance);
	playerBall.position += fallVector;
	playerBall.speed = reflectionVector * length(playerBall.speed) * staticBallBounceSpeedReduceCoef;
}

float32x2 StaticBall::GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge)
{
	switch (charge)
	{
	case Charge::Positive:
	case Charge::Negative:
	{
		float32x2 distanceVector = position - playerBall.position;
		float distance = length(distanceVector);
		return distanceVector * (charge == Charge::Positive ? staticBallChargeAbsValue : -staticBallChargeAbsValue)
			* playerBallCharge / (distance * distance * distance);
	}
	default: return float32x2(0.0f, 0.0f);
	}
}

void StaticBall::DrawForce(Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge)
{
	using namespace Render2D;

	float thisBallCharge = 0.0f;
	switch (charge)
	{
	case Charge::Positive: thisBallCharge = 1.0f; break;
	case Charge::Negative: thisBallCharge = -1.0f; break;
	default: return;
	}

	float playerBallDistance = length(position - playerBall.position);
	//float opacity = saturate(lincoef(staticBallDistanceForceOpac0, staticBallDistanceForceOpac1, length(position - playerBall.position)));
	float opacity = lincoefsatur(0.8f, 0.3f, playerBallDistance);
	if (opacity >= opacityThreshold)
	{
		uint32 color = thisBallCharge * playerBallCharge < 0.0f ? staticBallNegativeColor : staticBallPositiveColor;
		coloru32 colorCenter(color, opacity), colorSide(color, uint8(0));
		//batch->PushLine(position, playerBall.position, 0.03f, colorCenter);
		float width = lerp(0.01f, 0.08f, lincoefsatur(0.6f, 0.1f, playerBallDistance));
		batch->PushLineAligned(position, playerBall.position, width, colorSide, colorCenter, GradientType::Horizontal);
		batch->PushLineAligned(position, playerBall.position, -width, colorSide, colorCenter, GradientType::Horizontal);
	}
}

void StaticBall::Draw(Render2D::Batch* batch)
{
	using namespace Render2D;

	coloru32 color = 0;
	switch (charge)
	{
	case Charge::Positive: color = staticBallPositiveColor; break;
	case Charge::Negative: color = staticBallNegativeColor; break;
	default: color = staticBallNeutralColor; break;
	}
	//batch->PushCircleAA(position, radius, color);
	batch->PushCircleAA(position, radius * 0.8f, color);
	batch->PushCircleAA(position, radius, coloru32(colors::white), 0.89f);
}