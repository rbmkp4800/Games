#include "BallsGame.h"

#include "Random.h"

using namespace BallsGame;
using namespace Render2D;

constexpr float32 blursSpawnDistance = 1.5f;
constexpr uint32 backgroundColor = 0x00003f_rgb;
constexpr uint32 foregroundFarColor = Colors::DarkBlue;
constexpr uint32 foregroundNearColor = 0x3399ff_rgb;

//constexpr uint32 backgroundColor = 0x003f00_rgb;
//constexpr uint32 foregroundFarColor = Colors::DarkGreen;
//constexpr uint32 foregroundNearColor = 0x00c000_rgb;

void Background::spawnBlurs()
{
	while (nextBlurSpawnDelta <= 0.0f)
	{
		float32 depth = 0.3f + Random::GetFloat32(0.7f);
		float32 radius = (0.3f + Random::GetFloat32(0.8f)) / (depth * 5.0f + 1.0f);

		blursList.Insert(BlurDesc(float32x2(Random::GetFloat32(radius * 3.0f + 1.5f) - radius * 1.5f,
			nextBlurSpawnDelta + blursSpawnDistance), depth, radius));
		nextBlurSpawnDelta += Random::GetFloat32(0.3f);
	}
}

void Background::Initialize()
{
	nextBlurSpawnDelta = -1.0f;
	spawnBlurs();
}

void Background::UpdateAndDraw(float32 posDelta, float32 cameraDelta, Batch* batch)
{
	nextBlurSpawnDelta += posDelta;
	spawnBlurs();

	batch->GetDevice()->Clear(backgroundColor);

	StaticListEnumerator enumerator = blursList.GetFrontEnum();
	while (blursList.EnumIsValid(enumerator))
	{
		BlurDesc& desc = blursList.GetElement(enumerator);
		if (desc.position.y < hellDistance)
		{
			blursList.Delete(enumerator);
			continue;
		}

		Color color = Render2D::lerp(foregroundFarColor, foregroundNearColor, sqrval(desc.depth));
		desc.position.y += posDelta * (desc.depth * 0.7f + 0.2f);
		rectf32 circleRect = circle(desc.position, desc.radius);
		Color innerColor(color, 224);
		float32 focus = desc.depth * 0.7f;
		batch->PushEllipse(circleRect, innerColor, -0.0f, focus);
		batch->PushGradientEllipse(circleRect, innerColor, Color(color, 0), focus);
		//batch->PushEllipse(circleRect, colors::white, 0.95, 1.0f);

		blursList.Enumerate(enumerator);
	}
}