#include "BallsGame.h"

#include "Random.h"

using namespace BallsGame;
using namespace Render2D;

static const float blursSpawnDistance = 2.0f;

void Background::spawnBlurs()
{
	while (nextBlurSpawnDelta <= 0.0f)
	{
		float radius = 0.15f + Random::GetFloat(0.3f);
		blurs[blursCount].position.set(Random::GetFloat(radius * 3.0f + 1.0f) - radius * 1.5f,
			nextBlurSpawnDelta + blursSpawnDistance);
		blurs[blursCount].radius = radius;
		blurs[blursCount].posDeltaCoef = 0.3f + Random::GetFloat(0.4f);
		blurs[blursCount].color = Random::GetBool() ? colors::dodgerBlue : 0x370000;
		blursCount++;
		nextBlurSpawnDelta += Random::GetFloat(0.5f);
	}
}

void Background::Generate()
{
	nextBlurSpawnDelta = -1.0f;
	spawnBlurs();
}

void Background::UpdateAndDraw(float posDelta, Batch* batch)
{
	nextBlurSpawnDelta += posDelta;
	spawnBlurs();

	batch->GetDevice()->Clear(colors::darkBlue);
	for (uint32 i = 0; i < blursCount; i++)
	{
		blurs[i].move(posDelta);
		rectf32 circleRect = circle(blurs[i].position, blurs[i].radius);
		coloru32 innerColor = coloru32(blurs[i].color, 0.07f / blurs[i].radius);
		coloru32 outerColor = coloru32(blurs[i].color, uint8(0));
		//batch->PushEllipse(circleRect, innerColor);
		//batch->PushEllipse(circleRect, innerColor, -0.0f, 0.4f);
		//batch->PushGradientEllipse(circleRect, innerColor, outerColor, 0.4f);
		batch->PushEllipse(circleRect, innerColor, -0.0f, 0.8f);
		batch->PushGradientEllipse(circleRect, innerColor, outerColor, 0.8f);
	}
}