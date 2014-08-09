#include <memory.h>

#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

//-----------------------------------ICharacter---------------------------------//

GOUpdateResult ICharacter::Update(uint16 delta)
{
	if (!delta)
		return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(position.y + 1));;

	if (direction == Direction::None)
	{
		direction = GetNextDirection();
		if (direction == Direction::None)
			return GOUpdateResult(GOUpdateType::NoRedraw, GetGODrawPriority(positionF.y));
	}

	displacement += speed / 10000.0f * delta;

	if (displacement > 1.0f)
	{
		switch (direction)
		{
		case Direction::Up:
			position.y--;
			break;
		case Direction::Down:
			position.y++;
			break;
		case Direction::Left:
			position.x--;
			break;
		case Direction::Right:
			position.x++;
			break;
		}

		displacement -= 1.0f;
		if ((direction = GetNextDirection()) == Direction::None)
			displacement = 0.0f;
	}

	switch (direction)
	{
	case Direction::Up:
		positionF.set((float)position.x + 0.5f, (float)position.y + 1 - displacement);
		break;
	case Direction::Down:
		positionF.set((float)position.x + 0.5f, (float)position.y + 1 + displacement);
		break;
	case Direction::Left:
		positionF.set((float)position.x + 0.5f - displacement, (float)position.y + 1);
		break;
	case Direction::Right:
		positionF.set((float)position.x + 0.5f + displacement, (float)position.y + 1);
		break;
	default:
		positionF.set((float)position.x + 0.5f, (float)position.y + 1);
		break;
	}

	return GOUpdateResult(GOUpdateType::PositionChanged, GetGODrawPriority(positionF.y));
}

//----------------------------------Player-------------------------------------//

#define PLAYERANIM_FRAME_COUNT		4
#define PLAYERANIM_UPDATE_PERIOD	125

Player::Player(uint16 x, uint16 y) : ICharacter(x, y, 25), currentAnimFrame(0), animTick(0)
{
	Camera::SetFocusPoint(positionF);
}
GOUpdateResult Player::Update(uint16 delta)
{
	if (GameBase::playerControlsState.DropBomb())
	{
		GameBase::playerControlsState.DropBomb(false);
		try 
		{
			GameBase::goController1.Add(new Bomb((uint16) roundf32(positionF.x - 0.5f), (uint16) roundf32(positionF.y - 1.0f), 2500, 3));
		}
		catch (int) {}
	}

	GOUpdateResult result = ICharacter::Update(delta);
	if (result.type == GOUpdateType::PositionChanged)
		Camera::SetFocusPoint(positionF);

	if (direction != Direction::None)
	{
		animTick += delta;
		if (animTick > PLAYERANIM_UPDATE_PERIOD)
		{
			currentAnimFrame++;
			animTick = 0;
			if (currentAnimFrame >= PLAYERANIM_FRAME_COUNT)
				currentAnimFrame = 0;
		}
	}

	return result;
}
void Player::Draw()
{
	rectf32 destRect(positionF.x - 1.1f / 2.0f, positionF.y - 1.1f,
		positionF.x + 1.1f / 2.0f, positionF.y);
	Texture* texture;
	switch (direction)
	{
	case Direction::Down:
		texture = &resources.playerDownTexture;
		break;
	case Direction::Up:
		texture = &resources.playerUpTexture;
		break;
	case Direction::Left:
		texture = &resources.playerLeftTexture;
		break;
	case Direction::Right:
		texture = &resources.playerRightTexture;
		break;
	default:
		Render::DrawBitmap(&resources.playerStandTexture, &destRect);
		return;
	}
	Render::DrawBitmap(texture, &destRect, &GetTextureTileRect(PLAYERANIM_FRAME_COUNT, currentAnimFrame));
}

Direction Player::GetNextDirection()
{
	if (GameBase::playerControlsState.Up() && Field::current->GetBlock(position.x, position.y - 1)->IsGoable())
		return Direction::Up;
	if (GameBase::playerControlsState.Down() && Field::current->GetBlock(position.x, position.y + 1)->IsGoable())
		return Direction::Down;
	if (GameBase::playerControlsState.Left() && Field::current->GetBlock(position.x - 1, position.y)->IsGoable())
		return Direction::Left;
	if (GameBase::playerControlsState.Right() && Field::current->GetBlock(position.x + 1, position.y)->IsGoable())
		return Direction::Right;
	return Direction::None;
}

//----------------------------------IMonster-----------------------------------//

GOUpdateResult IMonster::Update(uint16 delta)
{
	GOUpdateResult result = ICharacter::Update(delta);
	if (Field::current->GetBlock(position)->GetFlags() & BLOCK_FLAG_FLAME)
		return GOUpdateResult(GOUpdateType::Release);
	return result;
}

Direction IMonster::GetNextDirection()
{
	if (lastPathUpdateFieldUid != Field::current->GetUID())
		UpdatePath();
	else if ((GameBase::player->GetPosition().x != targetPos.x || GameBase::player->GetPosition().y != targetPos.y)/* && !unreachable*/)
		UpdatePath();

	if (path[pathIdx] == Direction::None)
		return Direction::None;
	return path[pathIdx++];
}

static uint16 *lengthMatrix;
static uint16x2 *pathPointsStack;
static int16x2 fieldSize;

void IMonster::Init()
{
	fieldSize = Field::current->GetSize();
	lengthMatrix = new uint16[fieldSize.x * fieldSize.y];
	pathPointsStack = new uint16x2[fieldSize.x * fieldSize.y];
}

void IMonster::UpdatePath()
{
	memset(path, 0, sizeof(Direction) * MONSTER_PATHBUFF_SIZE);
	pathIdx = 0;
	unreachable = true;
	lastPathUpdateFieldUid = Field::current->GetUID();
	targetPos.set(GameBase::player->GetPosition().x, GameBase::player->GetPosition().y);

	memset(lengthMatrix, 0xff, sizeof(uint16) * fieldSize.x * fieldSize.y);
	uint16x2 *currPathPoint = pathPointsStack;
	uint16x2 *newPathPoint = pathPointsStack + 1;
	pathPointsStack->set(position.x, position.y);
	lengthMatrix[currPathPoint->x + currPathPoint->y * fieldSize.x] = 0;

	do
	{
		uint16 currLength = lengthMatrix[currPathPoint->x + currPathPoint->y * fieldSize.x] + 1;

		if (Field::current->GetBlock(currPathPoint->x, currPathPoint->y - 1)->IsGoable() &&
			lengthMatrix[currPathPoint->x + (currPathPoint->y - 1) * fieldSize.x] > currLength)
		{
			(newPathPoint++)->set(currPathPoint->x, currPathPoint->y - 1);
			lengthMatrix[currPathPoint->x + (currPathPoint->y - 1) * fieldSize.x] = currLength;
			if (currPathPoint->x == targetPos.x && currPathPoint->y - 1 == targetPos.y)
				unreachable = false;
		}
		if (Field::current->GetBlock(currPathPoint->x, currPathPoint->y + 1)->IsGoable() &&
			lengthMatrix[currPathPoint->x + (currPathPoint->y + 1) * fieldSize.x] > currLength)
		{
			(newPathPoint++)->set(currPathPoint->x, currPathPoint->y + 1);
			lengthMatrix[currPathPoint->x + (currPathPoint->y + 1) * fieldSize.x] = currLength;
			if (currPathPoint->x == targetPos.x && currPathPoint->y + 1 == targetPos.y)
				unreachable = false;
		}
		if (Field::current->GetBlock(currPathPoint->x - 1, currPathPoint->y)->IsGoable() &&
			lengthMatrix[currPathPoint->x - 1 + currPathPoint->y * fieldSize.x] > currLength)
		{
			(newPathPoint++)->set(currPathPoint->x - 1, currPathPoint->y);
			lengthMatrix[currPathPoint->x - 1 + currPathPoint->y * fieldSize.x] = currLength;
			if (currPathPoint->x - 1 == targetPos.x && currPathPoint->y == targetPos.y)
				unreachable = false;
		}
		if (Field::current->GetBlock(currPathPoint->x + 1, currPathPoint->y)->IsGoable() &&
			lengthMatrix[currPathPoint->x + 1 + currPathPoint->y * fieldSize.x] > currLength)
		{
			(newPathPoint++)->set(currPathPoint->x + 1, currPathPoint->y);
			lengthMatrix[currPathPoint->x + 1 + currPathPoint->y * fieldSize.x] = currLength;
			if (currPathPoint->x + 1 == targetPos.x && currPathPoint->y == targetPos.y)
				unreachable = false;
		}

		currPathPoint++;
	}
	while (currPathPoint < newPathPoint && unreachable);

	if (unreachable)
		return;

	uint16x2 currPos = targetPos;
	uint16 currLength = lengthMatrix[targetPos.x + targetPos.y * fieldSize.x] - 1;
	do
	{
		if (lengthMatrix[currPos.x + (currPos.y + 1) * fieldSize.x] == currLength)
		{
			currPos.y++;
			if (currLength < MONSTER_PATHBUFF_SIZE)
				path[currLength] = Direction::Up;
			currLength--;
		}
		else if (lengthMatrix[currPos.x + (currPos.y - 1) * fieldSize.x] == currLength)
		{
			currPos.y--;
			if (currLength < MONSTER_PATHBUFF_SIZE)
				path[currLength] = Direction::Down;
			currLength--;
		}
		else if (lengthMatrix[currPos.x + 1 + currPos.y * fieldSize.x] == currLength)
		{
			currPos.x++;
			if (currLength < MONSTER_PATHBUFF_SIZE)
				path[currLength] = Direction::Left;
			currLength--;
		}
		else if (lengthMatrix[currPos.x - 1 + currPos.y * fieldSize.x] == currLength)
		{
			currPos.x--;
			if (currLength < MONSTER_PATHBUFF_SIZE)
				path[currLength] = Direction::Right;
			currLength--;
		}
		else
			return;
	} while (currPos.x != targetPos.x || currPos.y != targetPos.y);
}

//-----------------------------------Monsters-----------------------------------//

#define GHOSTANIM_FRAME_COUNT	8
#define GHOSTANIM_UPDATE_PERIOD	70

GOUpdateResult Ghost::Update(uint16 delta)
{
	GOUpdateResult result = IMonster::Update(delta);

	animTick += delta;
	if (animTick > GHOSTANIM_UPDATE_PERIOD)
	{
		currentAnimFrame++;
		animTick = 0;
		if (currentAnimFrame >= GHOSTANIM_FRAME_COUNT)
			currentAnimFrame = 0;
		if (result.type == GOUpdateType::NoRedraw)
			result.type = GOUpdateType::Redraw;
	}
	
	return result;
}

void Ghost::Draw()
{
	Render::DrawBitmap(&resources.ghostTexture, &rectf32(positionF.x - 1.1f / 2.0f,
		positionF.y - 1.1f,	positionF.x + 1.1f / 2.0f, positionF.y),
		&GetTextureTileRect(GHOSTANIM_FRAME_COUNT, currentAnimFrame));
}

GOUpdateResult Jock::Update(uint16 delta)
{
	GOUpdateResult result = IMonster::Update(delta);

	animTick += delta;
	if (animTick > 100)
	{
		currentAnimFrame += animStep;
		animTick = 0;
		if (currentAnimFrame >= 3)
			animStep = -1;
		if (!currentAnimFrame)
			animStep = 1;
		if (result.type == GOUpdateType::NoRedraw)
			result.type = GOUpdateType::Redraw;
	}
	
	return result;
}

void Jock::Draw()
{
	Texture* bitmap;
	switch (direction)
	{
	case Direction::Up:
		bitmap = &resources.jockUpTexture;
		break;
	case Direction::Down:
		bitmap = &resources.jockDownTexture;
		break;
	default:
		Render::DrawBitmap(&resources.jockStandTexture, &rectf32(positionF.x - 1.3f / 2.0f, 
			positionF.y - 1.3f,	positionF.x + 1.3f / 2.0f, positionF.y));
		return;
	}
	Render::DrawBitmap(bitmap, &rectf32(positionF.x - 1.3f / 2.0f,
		positionF.y - 1.3f,	positionF.x + 1.3f / 2.0f, positionF.y), &GetTextureTileRect(4, currentAnimFrame));
}