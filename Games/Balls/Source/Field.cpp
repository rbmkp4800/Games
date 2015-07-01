#include "BallsGame.h"

#include "Random.h"

using namespace BallsGame;

inline void* operator new(size_t size, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

static const float objectsSpawnDistance = 1.5f;
static const uint32 minStaticBallsGroupSize = 2;
static const uint32 staticBallsGroupSizeDelta = 2;
static const float staticBallsGroupBallsMaxDelta = 0.1f;
static const float staticBallsGroupsMaxDelta = 0.3f;
static const float staticBallDefaulRadius = 0.045f;

void Field::spawnGameObjects()
{
	while (nextStaticBallsGroupSpawnDelta <= 0.0f)
	{
		uint32 groupBallsCount = minStaticBallsGroupSize + Random::GetUint32(staticBallsGroupSizeDelta + 1);
		for (uint32 i = 0; i < groupBallsCount; i++)
		{
			if (staticBallsQueue.IsFull())
				return;
			staticBallsQueue.PushBack(StaticBall(staticBallDefaulRadius + Random::GetFloat(1.0f - 2.0f * staticBallDefaulRadius),
				nextStaticBallsGroupSpawnDelta + objectsSpawnDistance, staticBallDefaulRadius, nextStaticBallGroupCharge));
			nextStaticBallsGroupSpawnDelta += Random::GetFloat(staticBallsGroupBallsMaxDelta);
		}
		nextStaticBallsGroupSpawnDelta += Random::GetFloat(staticBallsGroupsMaxDelta);
		nextStaticBallGroupCharge = nextStaticBallGroupCharge == Charge::Positive ? Charge::Negative : Charge::Positive;
	}
}

void Field::Initialize()
{
	staticBallsQueue.Clear();
	nextStaticBallsGroupSpawnDelta = -1.0f;
	nextStaticBallGroupCharge = Random::GetBool() ? Charge::Negative : Charge::Positive;
	spawnGameObjects();
}

float32x2 Field::GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge)
{
	float32x2 force(0.0f, 0.0f);
	uint32 staticBallsCount = staticBallsQueue.GetElementsCount();
	for (uint32 i = 0; i < staticBallsCount; i++)
		force += staticBallsQueue[i].GetForceAppliedToPlayerBall(playerBall, playerBallCharge);
	return force;
}

void Field::CollideWithPlayerBall(PlayerBall& playerBall, float32x2 translation)
{
	for (;;)
	{
		float translationLength = length(translation);
		if (translationLength <= 0.0f)
			break;

		float collisionDistance = translationLength;
		uint32 collidedBallIndex = uint32(-1);
		uint32 staticBallsCount = staticBallsQueue.GetElementsCount();
		for (uint32 ballIndex = 0; ballIndex < staticBallsCount; ballIndex++)
		{
			float tempDistance = staticBallsQueue[ballIndex].GetPlayerBallCollisionDistance(playerBall, translation, translationLength);
			if (tempDistance < collisionDistance)
			{
				collisionDistance = tempDistance;
				collidedBallIndex = ballIndex;
			}
		}

		if (playerBall.position.x + translation.x > 1.0f - playerBall.radius &&
			playerBall.position.x <= 1.0f - playerBall.radius)
		{
			float transWallCollCoef = (1.0f - playerBall.radius - playerBall.position.x) / translation.x;
			if (translationLength * transWallCollCoef < collisionDistance)
			{
				playerBall.position.x = 1.0f - playerBall.radius;
				playerBall.position.y += translation.y * transWallCollCoef;
				translation *= 1.0f - transWallCollCoef;
				translation.x = -translation.x;
				playerBall.speed.x = -playerBall.speed.x * 0.5f;
				continue;
			}
		}
		else if (playerBall.position.x + translation.x < playerBall.radius &&
			playerBall.position.x >= playerBall.radius)
		{
			float transWallCollCoef = (playerBall.radius - playerBall.position.x) / translation.x;
			if (translationLength * transWallCollCoef < collisionDistance)
			{
				playerBall.position.x = playerBall.radius;
				playerBall.position.y += translation.y * transWallCollCoef;
				translation *= 1.0f - transWallCollCoef;
				translation.x = -translation.x;
				playerBall.speed.x = -playerBall.speed.x * 0.5f;
				continue;
			}
		}

		if (collidedBallIndex != uint32(-1))
			staticBallsQueue[collidedBallIndex].CollideWithPlayerBall(collisionDistance, playerBall, translation, translationLength);
		else
			break;
	}
	playerBall.position += translation;
}

void Field::UpdateAndDraw(float posDelta, Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge)
{
	uint32 staticBallsCount = staticBallsQueue.GetElementsCount();
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBallsQueue[i].Move(posDelta);

	while (staticBallsQueue.PeekFront().GetPosition().y < hellDistance)
		staticBallsQueue.PopFront();

	nextStaticBallsGroupSpawnDelta += posDelta;
	spawnGameObjects();

	staticBallsCount = staticBallsQueue.GetElementsCount();
	if (playerBallCharge != 0.0f)
		for (uint32 i = 0; i < staticBallsCount; i++)
			staticBallsQueue[i].DrawForce(batch, playerBall, playerBallCharge);
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBallsQueue[i].Draw(batch);
}