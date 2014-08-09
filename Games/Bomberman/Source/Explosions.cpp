#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

#define MAX_EXPL_COUNT	32

#define FLAME_TIMER		750
#define FLAME_FLAG_SET_TIMER	400
#define FLAME_SPEED		6.5f

#define FLAMETEX_TILE_COUNT	9
#define FLAMETEX_TILE_SIZE	(1.0f / (float)FLAMETEX_TILE_COUNT)

class Explosion
{
private:
	int16x2 position;
	uint8 upFlameLength, leftFlameLength, downFlameLength, rightFlameLength;
	float displacement, opacity;
	uint16 tick;
	uint8 power;
	bool flameFlag;

	inline uint8 CheckFlame(int8 xdispl, int8 ydispl)
	{
		int16x2 tmpPos = position;
		for (uint8 dist = 0; dist < power; dist++)
		{
			tmpPos.x += xdispl;
			tmpPos.y += ydispl;

			BlockDesc* blockDesc = Field::current->GetBlock(tmpPos);

			if (blockDesc->IsFireable())
				blockDesc->SetFlag(BLOCK_FLAG_FLAME);
			if (blockDesc->IsDestroyable())
			{
				switch (blockDesc->block)
				{
				case Block::Box:
					GameBase::goController2.Add(new ExpandingFadingSprite(&resources.boxDestructionTexture,
						&rectf32(tmpPos.x, tmpPos.y, tmpPos.x + 1, tmpPos.y + 1), 2.0f, 500, 2.0f));
					break;
				case Block::BrickWall:
					GameBase::goController2.Add(new ExpandingFadingSprite(&resources.brickDestructionTexture,
						&rectf32(tmpPos.x, tmpPos.y, tmpPos.x + 1, tmpPos.y + 1), 2.0f, 500, 2.0f));
					break;
				}
				GameBase::goController3.Add(new AnimatedExpandingFadingSprite(&resources.dustTexture,
					&rectf32(tmpPos.x - 0.5f, tmpPos.y - 0.5f, tmpPos.x + 1.5f, tmpPos.y + 1.5f), 0.5f, 1000, 4, 125, 2.0f));

				Field::current->SetBlock(tmpPos, Block::Empty);
				return dist + 1;
			}
			if (blockDesc->IsUndestroyable())
				return dist;
		}
		return power;
	}
	inline void ResetFlameFlag()
	{
		for (int16 i = 1; i <= upFlameLength; i++)
			Field::current->GetBlock(position.add(0, -i))->ResetFlag(BLOCK_FLAG_FLAME);
		for (int16 i = 1; i <= downFlameLength; i++)
			Field::current->GetBlock(position.add(0, i))->ResetFlag(BLOCK_FLAG_FLAME);
		for (int16 i = 1; i <= leftFlameLength; i++)
			Field::current->GetBlock(position.add(-i, 0))->ResetFlag(BLOCK_FLAG_FLAME);
		for (int16 i = 1; i <= rightFlameLength; i++)
			Field::current->GetBlock(position.add(i, 0))->ResetFlag(BLOCK_FLAG_FLAME);
	}

public:
	inline Explosion(){}
	inline Explosion(uint16 x, uint16 y, uint8 _power) : position(x, y), power(_power), tick(0),
		displacement(1.0f), opacity(1.0f), flameFlag(true)
	{
		BlockDesc *destBlock = Field::current->GetBlock(position);
		if (!destBlock)
			throw BOMBEREXCEP_WRONG_COORDS;
		if (destBlock->block != Block::Empty)
			throw BOMBEREXCEP_OBJ_ALREDY_EXIST;

		leftFlameLength = CheckFlame(-1, 0);
		rightFlameLength = CheckFlame(1, 0);
		upFlameLength = CheckFlame(0, -1);
		downFlameLength = CheckFlame(0, 1);

		GameBase::goController0.Add(new FadingSprite(&resources.floorExplosionMarkTexture,
			&rectf32(position.x - 0.4f, position.y - 0.4f + BLOCK_PROJ_SIZE, position.x + 1.4f, position.y + 1.4f + BLOCK_PROJ_SIZE), 30000));
		GameBase::goController2.Add(new ExpandingFadingSprite(&resources.explosionWaveTexture,
			&rectf32(position.x - 0.4f, position.y - 0.4f, position.x + 1.4f, position.y + 1.4f), 3.0f, 500));
		GameBase::goController3.Add(new AnimatedSprite(&resources.explosionCenterTexture,
			&rectf32(position.x - 0.6f, position.y - 0.6f, position.x + 1.6f, position.y + 1.6f), 7, 120));
	}

	inline bool Update(uint16 delta)
	{
		tick += delta;

		if (tick > FLAME_TIMER)
			return false;
		if (tick > FLAME_FLAG_SET_TIMER && flameFlag)
			ResetFlameFlag();

		displacement -= FLAME_SPEED * delta / (1000.0f * FLAMETEX_TILE_COUNT);
		opacity = (1.0f - (float)tick / FLAME_TIMER) * 2.0f;
		if (opacity > 1.0f)
			opacity = 1.0f;

		return true;
	}

	inline void DrawOverBlocksPart()
	{
		if (rightFlameLength)
		{
			if (rightFlameLength == power)
			{
				if (rightFlameLength > 1)
				{
					Render::DrawBitmap(&resources.flameTexture,
						&rectf32(position.x + 1, position.y, position.x + rightFlameLength, position.y + 1),
						&rectf32(displacement, 0.0f, (rightFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 1.0f), opacity);
				}
				Render::DrawBitmap(&resources.flameTexture,
					&rectf32(position.x + rightFlameLength, position.y, position.x + rightFlameLength + 1, position.y + 1), opacity, 0.0f, Direction::Right,
					&rectf32((rightFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 0.0f, rightFlameLength * FLAMETEX_TILE_SIZE + displacement, 1.0f));
			}
			else
			{
				Render::DrawBitmap(&resources.flameTexture,
					&rectf32(position.x + 1, position.y, position.x + rightFlameLength + 1, position.y + 1),
					&rectf32(displacement, 0.0f, rightFlameLength * FLAMETEX_TILE_SIZE + displacement, 1.0f), opacity);
			}
		}
		if (leftFlameLength)
		{
			if (leftFlameLength == power)
			{
				if (leftFlameLength > 1)
				{
					Render::DrawBitmap(&resources.flameTexture,
						&rectf32(position.x - leftFlameLength + 1, position.y, position.x, position.y + 1),
						&rectf32((leftFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 0.0f, displacement, 1.0f), opacity);
				}
				Render::DrawBitmap(&resources.flameTexture,
					&rectf32(position.x - leftFlameLength, position.y, position.x - leftFlameLength + 1, position.y + 1), 0.0f, opacity, Direction::Right,
					&rectf32(leftFlameLength * FLAMETEX_TILE_SIZE + displacement, 0.0f, (leftFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 1.0f));
			}
			else
			{
				Render::DrawBitmap(&resources.flameTexture,
					&rectf32(position.x - leftFlameLength, position.y, position.x, position.y + 1),
					&rectf32(leftFlameLength * FLAMETEX_TILE_SIZE + displacement, 0.0f, displacement, 1.0f), opacity);
			}
		}
		if (upFlameLength)
		{
			if (upFlameLength == power)
			{
				if (upFlameLength > 1)
				{
					Render::DrawBitmap<true>(&resources.flameTexture,
						&rectf32(position.x, position.y - upFlameLength + 1, position.x + 1, position.y),
						&rectf32(0.0f, (upFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 1.0f, displacement), opacity);
				}
				Render::DrawBitmap<true>(&resources.flameTexture,
					&rectf32(position.x, position.y - upFlameLength, position.x + 1, position.y - upFlameLength + 1), opacity, 0.0f, Direction::Up,
					&rectf32(0.0f, upFlameLength * FLAMETEX_TILE_SIZE + displacement, 1.0f, (upFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement));
			}
			else
			{
				Render::DrawBitmap<true>(&resources.flameTexture,
					&rectf32(position.x, position.y - upFlameLength, position.x + 1, position.y),
					&rectf32(0.0f, upFlameLength * FLAMETEX_TILE_SIZE + displacement, 1.0f, displacement), opacity);
			}
		}
	}
	inline void DrawUnderBlocksPart()
	{
		if (downFlameLength)
		{
			if (downFlameLength == power)
			{
				if (downFlameLength > 1)
				{
					Render::DrawBitmap<true>(&resources.flameTexture,
						&rectf32(position.x, position.y + 1, position.x + 1, position.y + downFlameLength),
						&rectf32(0.0f, displacement, 1.0f, (downFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement), opacity);
				}
				Render::DrawBitmap<true>(&resources.flameTexture,
					&rectf32(position.x, position.y + downFlameLength, position.x + 1, position.y + downFlameLength + 1), opacity, 0.0f, Direction::Down,
					&rectf32(0.0f, (downFlameLength - 1) * FLAMETEX_TILE_SIZE + displacement, 1.0f, downFlameLength * FLAMETEX_TILE_SIZE + displacement));
			}
			else
			{
				Render::DrawBitmap<true>(&resources.flameTexture,
					&rectf32(position.x, position.y + 1, position.x + 1, position.y + downFlameLength + 1),
					&rectf32(0.0f, displacement, 1.0f, downFlameLength * FLAMETEX_TILE_SIZE + displacement), opacity);
			}
		}
	}
};

//---------------------------------------Controller--------------------------------------------//

static Explosion explosionBase[MAX_EXPL_COUNT];
static int16 explosionCount;

void ExplosionsController::Clear()
{
	explosionCount = 0;
}

void ExplosionsController::Create(uint16 x, uint16 y, uint8 power)
{
	if (explosionCount < MAX_EXPL_COUNT)
		explosionBase[explosionCount++] = Explosion(x, y, power);
}

bool ExplosionsController::Update(uint16 delta)
{
	if (!explosionCount)
		return false;

	for (int16 currExpl = 0; currExpl < explosionCount; currExpl++)
		if (!explosionBase[currExpl].Update(delta))
			explosionBase[currExpl--] = explosionBase[--explosionCount];

	return true;
}

void ExplosionsController::DrawUnderBlocksPart()
{
	Render::SetSamplerMode(SamplerMode::Linear_TextAddressWrap);
	for (uint16 currExpl = 0; currExpl < explosionCount; currExpl++)
		explosionBase[currExpl].DrawUnderBlocksPart();
	Render::SetSamplerMode(SamplerMode::Default);
}

void ExplosionsController::DrawOverBlocksPart()
{
	Render::SetSamplerMode(SamplerMode::Linear_TextAddressWrap);
	for (uint16 currExpl = 0; currExpl < explosionCount; currExpl++)
		explosionBase[currExpl].DrawOverBlocksPart();
	Render::SetSamplerMode(SamplerMode::Default);
}