#include "BallsGame.h"

#include <Random.h>

using namespace BallsGame;
using namespace Render2D;

inline void* operator new(size_t size, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

constexpr float32 objectsSpawnDistance = 1.5f;
constexpr uint32 minStaticBallsGroupSize = 2;
constexpr uint32 staticBallsGroupSizeDelta = 2;
constexpr float32 staticBallsGroupBallsMaxDelta = 0.1f;
constexpr float32 staticBallsGroupsMaxDelta = 0.3f;
constexpr float32 staticBallDefaulRadius = 0.04f;

constexpr float32 playerBallDefaultRadius = 0.05f;

constexpr float32 physicsUpdateStepTime = 0.016f;

void Field::spawnGameObjects()
{
	while (nextStaticBallsGroupSpawnDelta <= 0.0f)
	{
		uint32 groupBallsCount = minStaticBallsGroupSize + Random::GetUint32(staticBallsGroupSizeDelta + 1);
		for (uint32 i = 0; i < groupBallsCount; i++)
		{
			if (staticBallsQueue.IsFull())
				return;
			staticBallsQueue.PushBack(StaticBall(staticBallDefaulRadius + Random::GetFloat32(1.0f - 2.0f * staticBallDefaulRadius),
				nextStaticBallsGroupSpawnDelta + objectsSpawnDistance, staticBallDefaulRadius, nextStaticBallGroupCharge));
			nextStaticBallsGroupSpawnDelta += Random::GetFloat32(staticBallsGroupBallsMaxDelta);
		}
		nextStaticBallsGroupSpawnDelta += 0.1f + Random::GetFloat32(staticBallsGroupsMaxDelta);
		nextStaticBallGroupCharge = nextStaticBallGroupCharge == Charge::Positive ? Charge::Negative : Charge::Positive;
	}
}

void Field::Initialize()
{
	staticBallsQueue.Clear();
	nextStaticBallsGroupSpawnDelta = -1.0f;
	nextStaticBallGroupCharge = Random::GetBool() ? Charge::Negative : Charge::Positive;
	spawnGameObjects();

	playerBall.position.set(0.5f, 0.0f);
	playerBall.speed.set(0.0f, 0.0f);
	playerBall.radius = playerBallDefaultRadius;

	staticBallsQueue.PushBack(StaticBall(0.55f, -0.655f, 0.6f, Charge::Neutral));

	residualUpdateTime = 0.0f;
}

float32 Field::Update(float32 timeDelta, float32x2 globalAcceleration, float32 playerBallCharge)
{
	static uint32 frameid = 0;
	frameid++;

	uint32 staticBallsCount = staticBallsQueue.GetElementsCount();

	playerBall.charge = playerBallCharge;

	/*timeDelta += residualUpdateTime;
	if (timeDelta < physicsUpdateStepTime)
	{
		residualUpdateTime = timeDelta;
		return 0.0f;
	}
	do*/
	{
		float32x2 acceleration = globalAcceleration;
		for (uint32 i = 0; i < staticBallsCount; i++)
			acceleration += staticBallsQueue[i].GetForceAppliedToObject(playerBall.position, playerBall.charge);

		float32x2 translation = (playerBall.speed + acceleration * (physicsUpdateStepTime / 2.0f)) * physicsUpdateStepTime;
		playerBall.speed += acceleration * physicsUpdateStepTime;

		for (;;)
		{
			float32 translationLength = length(translation);
			if (translationLength <= 0.0f)
				break;

			float32 collisionDistance = translationLength;
			uint32 collidedBallIndex = uint32(-1);
			for (uint32 ballIdx = 0; ballIdx < staticBallsCount; ballIdx++)
			{
				float32 tempDistance = staticBallsQueue[ballIdx].GetPlayerBallCollisionDistance(playerBall, translation, translationLength);
				if (tempDistance < collisionDistance)
				{
					collisionDistance = tempDistance;
					collidedBallIndex = ballIdx;
				}
			}

			if (playerBall.position.x + translation.x > 1.0f - playerBall.radius &&
				playerBall.position.x <= 1.0f - playerBall.radius)
			{
				float32 transWallCollCoef = (1.0f - playerBall.radius - playerBall.position.x) / translation.x;
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
				float32 transWallCollCoef = (playerBall.radius - playerBall.position.x) / translation.x;
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
		timeDelta -= physicsUpdateStepTime;
	}/* while (timeDelta >= physicsUpdateStepTime)*/;
	residualUpdateTime = timeDelta;

	float32 posDelta = playerBall.position.y;
	playerBall.position.y = 0.0f;

	for (uint32 ballIdx = 0; ballIdx < staticBallsCount; ballIdx++)
		staticBallsQueue[ballIdx].Move(posDelta);

	while ((!staticBallsQueue.IsEmpty()) && staticBallsQueue.PeekFront().GetPosition().y < hellDistance)
		staticBallsQueue.PopFront();

	nextStaticBallsGroupSpawnDelta -= posDelta;
	spawnGameObjects();

	return posDelta;
}

void Field::Draw(Batch* batch)
{
	if (playerBall.charge > 0.0f)
		batch->PushGradientEllipse(circle(playerBall.position, 0.6f), Color(Colors::DeepSkyBlue, 128), Color(Colors::DeepSkyBlue, 0));
	if (playerBall.charge < 0.0f)
		batch->PushGradientEllipse(circle(playerBall.position, 0.6f), Color(Colors::LightSalmon, 128), Color(Colors::LightSalmon, 0));

	if (playerBall.charge != 0.0f)
	{
		uint32 staticBallsCount = staticBallsQueue.GetElementsCount();
		for (uint32 ballIdx = 0; ballIdx < staticBallsCount; ballIdx++)
			staticBallsQueue[ballIdx].DrawForce(batch, playerBall, playerBall.charge);
	}
	uint32 staticBallsCount = staticBallsQueue.GetElementsCount();
	for (uint32 ballIdx = 0; ballIdx < staticBallsCount; ballIdx++)
		staticBallsQueue[ballIdx].Draw(batch);

	batch->PushCircleAA(playerBall.position, playerBall.radius * 0.85f, Colors::White);
	batch->PushCircleAA(playerBall.position, playerBall.radius, Colors::White, 0.92f);
}