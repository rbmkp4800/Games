#include <stdlib.h>
#include <time.h>
#include <memory.h>

#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

#define FIELD_BITMAP_BLOCK_SIZE		64
#define SHADOW_BTIMAP_BLOCK_SIZE	32
#define BLOCK_BITMAP_BLOCK_SIZE		64

#define FLOORBACKTEX_TILE_COUNT		4
#define FLOORFOREST1_TEX_TILE_COUNT	4
#define FLOORFOREST1_TEX_TILE_SIZE	(1.0f / (float)FLOORFOREST1_TEX_TILE_COUNT)
#define FLOORFOREST2_TEX_TILE_COUNT	5
#define WALLTEX_TILE_COUNT	3

Field *Field::current;

Field::Field(uint16 x, uint16 y) : dim(x, y), uid(0)
{
	blocksMatrix = (BlockDesc*)malloc((sizeof(BlockDesc) + sizeof(uint8)) * dim.x * dim.y);
	blocksStyleMatrix = (uint8*)((BlockDesc*)blocksMatrix + dim.x * dim.y);

	srand(4);
	//srand((uint32)time(nullptr));

	coloru32* floorBackInitData = (coloru32*) blocksMatrix;
	for (uint32 i = 0; i < (dim.x / 2) * (dim.y / 2); i++)
		floorBackInitData[i].set(rand() % 60, rand() % 96 + 80, rand() % 60);
	Texture floorBackgroundBitmap;
	floorBackgroundBitmap.Create(dim.x / 2, dim.y / 2, floorBackInitData);

	uint8 *floorForeStyleMatrix = (uint8*)blocksMatrix;
	memset(floorForeStyleMatrix, 0, sizeof(uint8) * dim.x * dim.y);
	for (int16 i = 0; i < dim.x * dim.y / 15; i++)
	{
		int16 ix = rand() % dim.x, iy = rand() % dim.y;
		uint8 maxIdx = rand() % FLOORFOREST1_TEX_TILE_COUNT;

		for (int16 nix = ix; nix < min(ix + maxIdx + 1, dim.x); nix++)
		{
			int8 maxVIdx = maxIdx - (nix - ix);
			for (int16 niy = iy; niy < min(iy + maxVIdx + 1, dim.y); niy++)
				floorForeStyleMatrix[niy * dim.x + nix] = maxVIdx - (niy - iy) + 1;
			for (int16 niy = iy - 1; niy >= max(iy - maxVIdx, 0); niy--)
				floorForeStyleMatrix[niy * dim.x + nix] = maxVIdx - (iy - niy) + 1;
		}

		for (int16 nix = ix - 1; nix >= max(ix - maxIdx, 0); nix--)
		{
			int8 maxVIdx = maxIdx - (ix - nix);
			for (int16 niy = iy; niy < min(iy + maxVIdx + 1, dim.y); niy++)
				floorForeStyleMatrix[niy * dim.x + nix] = maxVIdx - (niy - iy) + 1;
			for (int16 niy = iy - 1; niy >= max(iy - maxVIdx, 0); niy--)
				floorForeStyleMatrix[niy * dim.x + nix] = maxVIdx - (iy - niy) + 1;
		}
	}

	blocksBitmap.Create(dim.x * BLOCK_BITMAP_BLOCK_SIZE, dim.y * BLOCK_BITMAP_BLOCK_SIZE);
	floorBitmap.Create(dim.x * FIELD_BITMAP_BLOCK_SIZE, dim.y * FIELD_BITMAP_BLOCK_SIZE);
	shadowsBitmap.Create(dim.x * SHADOW_BTIMAP_BLOCK_SIZE, dim.y * SHADOW_BTIMAP_BLOCK_SIZE);

	Render::SetTarget(&floorBitmap);
	Render::SetTransform(matrix3x2::scale(FIELD_BITMAP_BLOCK_SIZE, FIELD_BITMAP_BLOCK_SIZE));
	Render::DrawBitmap(&floorBackgroundBitmap, &rectf32(0.0f, 0.0f, dim.x, dim.y));
	for (uint16 iy = 0; iy < dim.y; iy++)
	{
		for (uint16 ix = 0; ix < dim.x; ix++)
		{
			int id = rand() % FLOORBACKTEX_TILE_COUNT;
			Render::DrawBitmap(&resources.floorBackTexture, &rectf32(ix, iy, ix + 1, iy + 1),
				&GetTextureTileRect(FLOORBACKTEX_TILE_COUNT, id));
		}
	}
	for (uint16 iy = 0; iy < dim.y; iy++)
	{
		for (uint16 ix = 0; ix < dim.x; ix++)
		{
			if (rand() % 15 == 0)
			{
				uint8 tileIdx = rand() % FLOORFOREST2_TEX_TILE_COUNT;
				Render::DrawBitmap(&resources.floorForeStyle2Texture, &rectf32(ix, iy, ix + 1, iy + 1),
					&GetTextureTileRect(FLOORFOREST2_TEX_TILE_COUNT, tileIdx));
			}
			else if (floorForeStyleMatrix[iy * dim.x + ix])
			{
				rectf32 srcRect;
				srcRect.left = srcRect.right = (floorForeStyleMatrix[iy * dim.x + ix] - 1) * FLOORFOREST1_TEX_TILE_SIZE;
				srcRect.right += FLOORFOREST1_TEX_TILE_SIZE;
				if (rand() % 2)
				{
					srcRect.top = 0.0f;
					srcRect.bottom = 1.0f;
				}
				else
				{
					srcRect.top = 1.0f;
					srcRect.bottom = 0.0f;
				}
				Render::DrawBitmap(&resources.floorForeStyle1Texture, &rectf32(ix, iy, ix + 1, iy + 1), &srcRect);
			}
		}
	}

	memset(blocksMatrix, 0, sizeof(BlockDesc) * x * y);
	memset(blocksStyleMatrix, 0, sizeof(uint8) * x * y);
	for (uint16 iy = 0; iy < dim.y; iy++)
	{
		for (uint16 ix = 0; ix < dim.x; ix++)
		{
			if (ix % 3 == 0 && iy % 3 == 0)
			{
				blocksMatrix[iy * dim.x + ix].block = Block::Wall;
				blocksStyleMatrix[iy * dim.x + ix] = rand() % WALLTEX_TILE_COUNT;
			}
			else if (rand() % 5 == 0)
				blocksMatrix[iy * dim.x + ix].block = (rand() % 2) ? Block::Box : Block::BrickWall;
		}
	}

	blocksMatrix[0].block = Block::Empty;
	blocksMatrix[1].block = Block::Empty;

	RedrawBlocks();
}

//---------------------------------Blocks Drawing-------------------------------------//

#define SHADOW_SIZE		0.25f
#define SUN_PROJ_SIZE	(BLOCK_PROJ_SIZE * 0.5f)
#define SHADOW_COLOR	0x000017
#define SHADOW_OPACITYF	0.3f
#define SHADOW_OPACITY	((uint8)(SHADOW_OPACITYF * 255.0f))

void Field::RedrawBlocks()
{
	Render::SetTarget(&blocksBitmap);
	Render::Clear(colors::transparent);

	Render::SetTransform(matrix3x2::scale(BLOCK_BITMAP_BLOCK_SIZE, BLOCK_BITMAP_BLOCK_SIZE));
	for (uint16 iy = 0; iy < dim.y; iy++)
	{
		for (uint16 ix = 0; ix < dim.x; ix++)
		{
			switch (blocksMatrix[iy * dim.x + ix].block)
			{
			case Wall:
				Render::DrawBitmap(&resources.wallTexture, &rectf32(ix, iy, ix + 1, BLOCK_PROJ_SIZE + iy + 1),
					&GetTextureTileRect(WALLTEX_TILE_COUNT, blocksStyleMatrix[iy * dim.x + ix]));
				break;
			case Box:
				Render::DrawBitmap(&resources.boxTexture, &rectf32(ix, iy, ix + 1, BLOCK_PROJ_SIZE + iy + 1));
				break;
			case BrickWall:
				Render::DrawBitmap(&resources.brickWallTexture, &rectf32(ix, iy, ix + 1, BLOCK_PROJ_SIZE + iy + 1));
				break;
			}
		}
	}

	//Core::polygonsDrawingBatch.Clear();
	//Core::polygonsDrawingBatch.transform = Matrix::Identity();
	//for (uint16 iy = 0; iy < dim.y; iy++)
	//{
	//	for (uint16 ix = 1; ix < dim.x; ix++)
	//	{
	//		if (GetBlock(ix, iy)->IsHigh()
	//			&& GetBlock(ix - 1, iy - 1)->IsHigh()
	//			&& !GetBlock(ix - 1, iy)->IsHigh())
	//		{
	//			float32x2 vb[3] = 
	//			{
	//				float32x2(ix - SHADOW_SIZE, iy + BLOCK_PROJ_SIZE),
	//				float32x2(ix, iy),
	//				float32x2(ix, iy + BLOCK_PROJ_SIZE),
	//			};
	//			Core::polygonsDrawingBatch.PushTrinagle(vb, Color(SHADOW_COLOR, SHADOW_OPACITY));
	//		}
	//	}
	//}
	//Device::DrawBatch(&Core::polygonsDrawingBatch);

	//Core::polygonsDrawingBatch.transform = Matrix::Scale(SHADOW_BTIMAP_BLOCK_SIZE, SHADOW_BTIMAP_BLOCK_SIZE);
	//Core::polygonsDrawingBatch.Clear();
	//for (uint16 iy = 0; iy < dim.y; iy++)
	//{
	//	for (uint16 ix = 0; ix < dim.x; ix++)
	//	{
	//		if (!GetBlock(ix, iy)->IsHigh() && GetBlock(ix + 1, iy)->IsHigh())
	//		{
	//			rectf32 rect(ix + 1 - SHADOW_SIZE, iy, ix + 1, iy + 1);

	//			/*if (GetBlock(ix, iy - 1)->IsHigh())
	//			{
	//				float32x2 vb[3] = 
	//				{
	//					float32x2(ix + 1 - SHADOW_SIZE, iy + BLOCK_PROJ_SIZE),
	//					float32x2(ix + 1, iy),
	//					float32x2(ix + 1, iy + BLOCK_PROJ_SIZE),
	//				};
	//				Device::FastColorDraw::DrawTriang(vb, Color(SHADOW_COLOR, 0xff));
	//				rect.top = iy + BLOCK_PROJ_SIZE;
	//			}
	//			else*/

	//			if (!GetBlock(ix + 1, iy - 1)->IsHigh())
	//				rect.top += SUN_PROJ_SIZE - BLOCK_PROJ_SIZE;

	//			/*if (GetBlock(ix, iy + 1)->IsHigh())
	//				rect.bottom = iy + 1;
	//			else*/

	//			if (!GetBlock(ix + 1, iy + 1)->IsHigh())
	//			{
	//				rect.bottom += SUN_PROJ_SIZE - BLOCK_PROJ_SIZE;
	//				float32x2 vb[3] = 
	//				{
	//					float32x2(ix + 1 - SHADOW_SIZE, iy + 1 + SUN_PROJ_SIZE - BLOCK_PROJ_SIZE),
	//					float32x2(ix + 1, iy + 1 + SUN_PROJ_SIZE - BLOCK_PROJ_SIZE),
	//					float32x2(ix + 1, iy + 1),
	//				};
	//				Core::polygonsDrawingBatch.PushTrinagle(vb, Color(SHADOW_COLOR, 0xff));
	//			}

	//			Core::polygonsDrawingBatch.PushRect(&rect, Color(SHADOW_COLOR, 0xff));
	//		}
	//	}
	//}
	//Device::SetTarget(&shadowsBitmap);
	//Device::Clear(Color(Colors::Transparent));
	//Device::DrawBatch(&Core::polygonsDrawingBatch);
}

void Field::Update()
{
	if (changed)
	{
		RedrawBlocks();

		uid++;
		changed = false;
	}
}
void Field::DrawFloor()
{
	Render::DrawBitmap(&floorBitmap, &rectf32(0.0f, BLOCK_PROJ_SIZE, dim.x, dim.y + BLOCK_PROJ_SIZE));
	Render::DrawBitmap(&shadowsBitmap, &rectf32(0.0f, BLOCK_PROJ_SIZE, dim.x, dim.y + BLOCK_PROJ_SIZE), nullptr, SHADOW_OPACITYF);
}
void Field::DrawBlocks()
{
	Render::DrawBitmap(&blocksBitmap, &rectf32(0.0f, 0.0f, dim.x, dim.y));
}