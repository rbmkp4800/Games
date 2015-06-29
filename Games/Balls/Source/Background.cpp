#include "BallsGame.h"

#include "Random.h"

using namespace BallsGame;
using namespace Render2D;

static const float blursSpawnDistance = 2.0f;

void Background::spawnBlurs()
{
	while (nextBlurSpawnDelta <= 0.0f)
	{
		float depth = Random::GetFloat(1.0f);
		float radius = (0.15f + Random::GetFloat(0.8f)) / (depth * 6.0f + 1.0f);

		BlurDesc desc;
		desc.position.set(Random::GetFloat(radius * 3.0f + 1.5f) - radius * 1.5f, nextBlurSpawnDelta + blursSpawnDistance);
		desc.depth = depth;
		desc.radius = radius;

		blursList.Insert(desc);
		nextBlurSpawnDelta += Random::GetFloat(0.3f);
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

	batch->GetDevice()->Clear(0xff400000);

	StaticListEnumerator enumerator = blursList.GetFrontEnum();
	while (blursList.EnumIsValid(enumerator))
	{
		BlurDesc& desc = blursList.GetElement(enumerator);
		if (desc.position.y > blursSpawnDistance || desc.position.y < -blursSpawnDistance)
		{
			blursList.Delete(enumerator);
			continue;
		}

		coloru32 color = Render2D::lerp(coloru32(colors::darkBlue), coloru32(0xffFF9933), sqrval(desc.depth));
		desc.position.y += posDelta * (desc.depth * 0.7f + 0.2f);
		rectf32 circleRect = circle(desc.position, desc.radius);
		coloru32 innerColor(color.rgba, uint8(224));
		coloru32 outerColor(color.rgba, uint8(0));
		float focus = desc.depth * 0.9f;
		batch->PushEllipse(circleRect, innerColor, -0.0f, focus);
		batch->PushGradientEllipse(circleRect, innerColor, outerColor, focus);
		//batch->PushEllipse(circleRect, colors::white, 0.95, 1.0f);

		blursList.Enumerate(enumerator);
	}
}