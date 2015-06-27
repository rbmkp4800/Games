#include "BallsGame.h"

#include "Random.h"

using namespace BallsGame;
using namespace Render2D;

static const float blursSpawnDistance = 2.0f;
static const float blursDeleteDistance = -2.0f;

void Background::spawnBlurs()
{
	while (nextBlurSpawnDelta <= 0.0f)
	{
		float depth = Random::GetFloat(1.0f);
		for ()
		float32x3 position(;


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

void Background::Initialize()
{
	nextBlurSpawnDelta = -1.0f;
	spawnBlurs();
}

void Background::UpdateAndDraw(float posDelta, float cameraDelta, Batch* batch)
{
	nextBlurSpawnDelta += posDelta;
	spawnBlurs();

	batch->GetDevice()->Clear(colors::darkBlue);
	for (uint32 i = 0; i < blursCount; i++)
	{
		blurs[i].move(posDelta);
		rectf32 circleRect = circle(blurs[i].position + float32x2(0.0f, cameraDelta * blurs[i].posDeltaCoef), blurs[i].radius);
		coloru32 innerColor = coloru32(blurs[i].color, 0.12f / blurs[i].radius);
		coloru32 outerColor = coloru32(blurs[i].color, uint8(0));
		//batch->PushEllipse(circleRect, innerColor);
		//batch->PushEllipse(circleRect, innerColor, -0.0f, 0.4f);
		//batch->PushGradientEllipse(circleRect, innerColor, outerColor, 0.4f);
		batch->PushEllipse(circleRect, innerColor, -0.0f, 0.6f);
		batch->PushGradientEllipse(circleRect, innerColor, outerColor, 0.6f);
	}
}