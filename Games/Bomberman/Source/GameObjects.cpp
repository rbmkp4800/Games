#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

//----------------------------------------------Bomb---------------------------------------------//

#define BOMB_START_SIZE	0.8f

#define BOMB_FLUC_CNT_BEFORE_EXPL	4.3f
#define BOMB_FLUC_MAX_EXPANSION		0.4f

Bomb::Bomb(uint16 x, uint16 y, uint16 _timer, uint8 _power)
	: position(x, y), power(_power), timer(_timer), tick(0)
{
	BlockDesc *destBlock = Field::current->GetBlock(position);
	if (!destBlock)
		throw BOMBEREXCEP_WRONG_COORDS;
	if (destBlock->block != Block::Empty)
		throw BOMBEREXCEP_OBJ_ALREDY_EXIST;

	Field::current->SetBlock(position, Block::BombPlace, this);
}

GOUpdateResult Bomb::Update(uint16 delta)
{
	if (!delta)
		return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(position.y + 1));;

	tick += delta;
	if (tick >= timer || Field::current->GetBlock(position)->GetFlags() & BLOCK_FLAG_FLAME)
	{
		Camera::Shake();
		Field::current->SetBlock(position, Block::Empty);
		ExplosionsController::Create(position.x, position.y, power);
		return GOUpdateResult(GOUpdateType::Release);
	}
	return GOUpdateResult(GOUpdateType::Redraw, GetGODrawPriority(position.y + 1));
}

void Bomb::Draw()
{
	float state = tick / (float)timer;
	float currentExpansion = BOMB_FLUC_MAX_EXPANSION / 2.0f * (sinf(state * BOMB_FLUC_CNT_BEFORE_EXPL * PI) + 1.0f) * state;
	rectf32 destRect(
		position.x + (1.0f - BOMB_START_SIZE) / 2.0f - currentExpansion,
		position.y + 1.0f - BOMB_START_SIZE - currentExpansion * 2.0f,
		position.x + (1.0f + BOMB_START_SIZE) / 2.0f + currentExpansion,
		position.y + 1.0f);

	Render::DrawBitmap(&resources.bombTexture, &destRect);
}

//------------------------------------------Sprites-------------------------------------------//

GOUpdateResult AnimatedSprite::Update(uint16 delta)
{
	if (!delta)
		return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(destRect.bottom));

	tick += delta;
	if (tick > period)
	{
		currentFrame++;
		if (currentFrame >= frameCount)
			return GOUpdateResult(GOUpdateType::Release);
		tick = 0;
		return GOUpdateResult(GOUpdateType::Redraw, GetGODrawPriority(destRect.bottom));
	}
	return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(destRect.bottom));
}
void AnimatedSprite::Draw()
{
	Render::DrawBitmap(bitmap, &destRect, &GetTextureTileRect(frameCount, currentFrame));
}

GOUpdateResult ExpandingFadingSprite::Update(uint16 delta)
{
	if (!delta)
		return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(destRect.bottom));

	tick += delta;
	return tick > timer ? GOUpdateResult(GOUpdateType::Release)
		: GOUpdateResult(GOUpdateType::Redraw, GetGODrawPriority(destRect.bottom));
}
void ExpandingFadingSprite::Draw()
{
	float state = tick / (float)timer;
	float currentExpansion = expansion * state;
	Render::DrawBitmap(bitmap, &rectf32(destRect.left - currentExpansion, destRect.top - currentExpansion,
		destRect.right + currentExpansion, destRect.bottom + currentExpansion), nullptr, (1.0f - state) * opacity);
}

GOUpdateResult AnimatedExpandingFadingSprite::Update(uint16 delta)
{
	if (!delta)
		return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(destRect.bottom));

	if (animMode != AnimatedExpandingFadingSprite::Stopped)
	{
		animTick += delta;
		if (animTick > animPeriod)
		{
			currentFrame++;
			animTick = 0;
			if (currentFrame >= frameCount)
			{
				if (animMode == AnimatedExpandingFadingSprite::Cycled)
					currentFrame = 0;
				else
				{
					animMode = AnimatedExpandingFadingSprite::Stopped;
					currentFrame--;
				}
			}
		}
	}

	tick += delta;
	return tick > timer ? GOUpdateResult(GOUpdateType::Release)
		: GOUpdateResult(GOUpdateType::Redraw, GetGODrawPriority(destRect.bottom));
}
void AnimatedExpandingFadingSprite::Draw()
{
	float state = tick / (float)timer;
	float currentExpansion = expansion * state;
	Render::DrawBitmap(bitmap, &rectf32(destRect.left - currentExpansion, destRect.top - currentExpansion,
		destRect.right + currentExpansion, destRect.bottom + currentExpansion),
		&GetTextureTileRect(frameCount, currentFrame), (1.0f - state) * opacity);
}

#define FADINGSPRITE_REDRAW_COUNT	40

FadingSprite::FadingSprite(Render2D::Texture* _bitmap, rectf32* _destRect, uint32 _timer, float _opacity)
	: bitmap(_bitmap), destRect(*_destRect), timer(_timer), opacity(_opacity),
	tick(0), lastOpacity(FADINGSPRITE_REDRAW_COUNT) {}

GOUpdateResult FadingSprite::Update(uint16 delta)
{
	tick += delta;

	if (tick >= timer)
		return GOUpdateResult(GOUpdateType::Release);

	uint8 newOpacity = FADINGSPRITE_REDRAW_COUNT - tick * FADINGSPRITE_REDRAW_COUNT / timer;
	if (newOpacity != lastOpacity)
	{
		lastOpacity = newOpacity;
		return GOUpdateResult(GOUpdateType::Redraw);
	}
	return GOUpdateResult(GOUpdateType::NoRedraw);
}
void FadingSprite::Draw()
{
	Render::DrawBitmap(bitmap, &destRect, nullptr, lastOpacity * opacity / (float)FADINGSPRITE_REDRAW_COUNT);
}