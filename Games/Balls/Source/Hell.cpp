#include "BallsGame.h"

#include "Random.h"

using namespace Render2D;
using namespace BallsGame;

void Hell::spawnBlurs()
{
	while (nextBlurSpawnDelta >= 0.0f)
	{
		if (blursQueue.IsFull())
			return;

		BlurDesc desc;
		desc.position.set(Random::GetFloat(1.0f), 0.0f);
		desc.radius = 0.1f + Random::GetFloat(0.2f);
		desc.lifeTime = 0.8f;
		blursQueue.PushBack(desc);

		nextBlurSpawnDelta -= 0.03f + Random::GetFloat(0.05f);
	}
}

void Hell::Initialize()
{
	distance = hellDistance;
	blursQueue.Clear();
	nextBlurSpawnDelta = 0.0f;
	spawnBlurs();
}

void Hell::UpdateAndDraw(float posDelta, float timeDelta, Render2D::Batch* batch)
{
	if (timeDelta > 0.1f)
		timeDelta = 0.1f;

	distance += posDelta;
	if (distance < hellDistance)
		distance = hellDistance;
	batch->PushGradientRect(rectf32(0.0f, distance + 1.0f, 1.0f, distance), coloru32(colors::red, uint8(128)), coloru32(colors::red, uint8(0)), GradientType::Vertical);
	batch->PushGradientRect(rectf32(0.0f, distance + 0.2f, 1.0f, distance), colors::lightYellow, coloru32(colors::lightYellow, uint8(0)), GradientType::Vertical);
	batch->PushRectangle(rectf32(0.0f, distance, 1.0f, -10.0f), colors::lightYellow);

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
		rectf32 rect = circle(float32x2(desc.position.x, desc.position.y + distance - 0.15f), desc.radius * (desc.lifeTime + 0.2f));
		coloru32 color(colors::lightYellow, desc.lifeTime);
		batch->PushEllipse(rect, color, -0.0f, 0.5f);
		batch->PushGradientEllipse(rect, color, coloru32(colors::lightYellow, uint8(0)), 0.5f);
	}
}