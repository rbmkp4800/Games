#include "BallsGame.h"

#include <Random.h>

using namespace Render2D;
using namespace BallsGame;

void Hell::spawnBlurs()
{
	while (nextBlurSpawnDelta >= 0.0f)
	{
		if (blursQueue.IsFull())
			return;
		blursQueue.PushBack(BlurDesc(float32x2(Random::GetFloat32(1.0f), 0.0f), 0.1f + Random::GetFloat32(0.2f), 0.8f));
		nextBlurSpawnDelta -= 0.02f + Random::GetFloat32(0.05f);
	}
}

void Hell::Initialize()
{
	distance = hellDistance;
	blursQueue.Clear();
	nextBlurSpawnDelta = 0.0f;
	spawnBlurs();
}

void Hell::UpdateAndDraw(float32 posDelta, float32 timeDelta, Render2D::Batch* batch)
{
	if (timeDelta > 0.1f)
		timeDelta = 0.1f;

	distance -= posDelta;
	if (distance < hellDistance)
		distance = hellDistance;
	batch->PushGradientRect(rectf32(0.0f, distance + 1.0f, 1.0f, distance), Color(Colors::Red, 128), Color(Colors::Red, 0), GradientType::Vertical);
	batch->PushGradientRect(rectf32(0.0f, distance + 0.2f, 1.0f, distance), Colors::LightYellow, Color(Colors::LightYellow, 0), GradientType::Vertical);
	batch->PushRectangle(rectf32(0.0f, distance, 1.0f, -10.0f), Colors::LightYellow);

	while ((!blursQueue.IsEmpty()) && blursQueue.PeekFront().lifeTime <= 0.0f)
		blursQueue.PopFront();

	nextBlurSpawnDelta += timeDelta;
	spawnBlurs();

	uint32 blursCount = blursQueue.GetElementsCount();
	for (uint32 blurIndex = 0; blurIndex < blursCount; blurIndex++)
	{
		BlurDesc& desc = blursQueue[blurIndex];
		desc.position.y += timeDelta;
		desc.lifeTime -= timeDelta;
		rectf32 rect = circle(float32x2(desc.position.x, desc.position.y + distance - 0.15f), desc.radius * (desc.lifeTime + 0.4f));
		batch->PushGradientEllipse(rect, Color(Colors::LightYellow, unormFloat32toUint8(saturate(desc.lifeTime))), Color(Colors::LightYellow, 0));
	}
}