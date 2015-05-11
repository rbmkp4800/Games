#include "BallsGame.h"

using namespace BallsGame;

inline void* operator new(size_t size, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

void Field::Clear()
{
	staticBallsCount = 5;
	new(staticBalls + 0) StaticBall(0.585f, 1.2f, 0.04f, -1.0f);
	new(staticBalls + 1) StaticBall(0.7f, 0.7f, 0.04f, 1.0f);
	new(staticBalls + 2) StaticBall(0.2f, 0.5f, 0.04f, 1.0f);
	new(staticBalls + 3) StaticBall(0.4f, 0.9f, 0.04f, -1.0f);
	new(staticBalls + 4) StaticBall(0.51f, 1.2f, 0.04f, 1.0f);
}

float32x2 Field::GetForceAppliedToPlayerBall(const PlayerBall& playerBall)
{
	float32x2 force(0.0f, 0.0f);
	for (uint32 i = 0; i < staticBallsCount; i++)
		force += staticBalls[i].GetForceAppliedToPlayerBall(playerBall);
	return force;
}

void Field::CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& translation)
{
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBalls[i].CollideWithPlayerBall(timeDelta, playerBall, translation);
}

void Field::UpdateAndDraw(float posDelta, Render2D::Batch* batch)
{
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBalls[i].Move(posDelta);
	for (uint32 i = 0; i < staticBallsCount; i++)
		staticBalls[i].Draw(batch);
}