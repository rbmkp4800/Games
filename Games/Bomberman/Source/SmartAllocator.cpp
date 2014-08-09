#include <memory.h>
#include <malloc.h>
#include "types.h"

#include "Bomberman.SmartAllocator.h"

using namespace Bomberman;

#define SMARTALLOC_PAGE_SIZE		16
#define SMARTALLOC_BLOCK_SIZE		4096
#define SMARTALLOC_PAGES_PER_BLOCK	(SMARTALLOC_BLOCK_SIZE / SMARTALLOC_PAGE_SIZE)
#define SMARTALLOC_BLOCK_COUNT		16

static void *allocBlockBase[SMARTALLOC_BLOCK_COUNT];
static uint8 *segmSizeBase[SMARTALLOC_BLOCK_COUNT];
static uint8 allocatedBlockCount;
union Map
{
	uint64 allocMapBase64[SMARTALLOC_BLOCK_COUNT][SMARTALLOC_PAGES_PER_BLOCK / 64];
	uint8 allocMapBase8[SMARTALLOC_BLOCK_COUNT][SMARTALLOC_PAGES_PER_BLOCK / 8];
} static allocMapBase;

inline void SetMapBits(uint8 block, uint8 begin, uint8 length, bool state)
{
	uint8 end = begin + length;
	uint8 begin64 = begin / 64, end64 = end / 64;
	if (state)
	{
		if (begin64 < end64)
		{
			allocMapBase.allocMapBase64[block][begin64] |= UINT64_MAX << (begin % 64);
			allocMapBase.allocMapBase64[block][end64] |= UINT64_MAX >> (64 - end % 64);
			if (begin64 + 1 < end64)
			{
				for (uint8 i = begin64 + 1; i < end64; i++)
					allocMapBase.allocMapBase64[block][i] = UINT64_MAX;
			}
		}
		else
			allocMapBase.allocMapBase64[block][begin64] |= (UINT64_MAX << (begin % 64)) & (UINT64_MAX >> (64 - end % 64));
	}
	else
	{
		if (begin64 < end64)
		{
			allocMapBase.allocMapBase64[block][begin64] &= ~(UINT64_MAX << (begin % 64));
			allocMapBase.allocMapBase64[block][end64] &= ~(UINT64_MAX >> (64 - end % 64));
			if (begin64 + 1 < end64)
			{
				for (uint8 i = begin64 + 1; i < end64; i++)
					allocMapBase.allocMapBase64[block][i] = 0;
			}
		}
		else
			allocMapBase.allocMapBase64[block][begin64] &= ~((UINT64_MAX << (begin % 64)) & (UINT64_MAX >> (64 - end % 64)));
	}
}

inline bool GetMapBit(uint8 block, uint8 page)
{
	return (allocMapBase.allocMapBase64[block][page / 64] >> (page % 64)) & 1;
}

inline uint8 ReverseFindFirstSetBitInMap8(uint8 block, uint8 byte, int8 start = 8)
{
	uint8 map = allocMapBase.allocMapBase8[block][byte];
	for (int8 curr = start - 1; curr >= 0; curr--)
	{
		if ((map >> curr) & 1)
			return curr;
	}
	return UINT8_MAX;
}

void AllocateNewBlock()
{
	allocBlockBase[allocatedBlockCount] = malloc(SMARTALLOC_BLOCK_SIZE);
	segmSizeBase[allocatedBlockCount] = (uint8*) malloc(SMARTALLOC_PAGES_PER_BLOCK);
	segmSizeBase[allocatedBlockCount][0] = SMARTALLOC_PAGES_PER_BLOCK - 1;
	memset(segmSizeBase[allocatedBlockCount] + 1, 0, SMARTALLOC_PAGES_PER_BLOCK - 1);
	allocatedBlockCount++;
}

void SmartAllocator::Init()
{
	allocatedBlockCount = 0;
	memset(allocMapBase.allocMapBase64, 0, SMARTALLOC_BLOCK_COUNT * SMARTALLOC_PAGES_PER_BLOCK / 8);
	AllocateNewBlock();
}

void* SmartAllocator::Allocate(uint16 size)
{
	if (size > SMARTALLOC_BLOCK_SIZE)
		return nullptr;
	uint8 pages = size / SMARTALLOC_PAGE_SIZE;
	if (!(size % SMARTALLOC_PAGE_SIZE)) pages--;

	for (uint8 currBlockIdx = 0; currBlockIdx < allocatedBlockCount; currBlockIdx++)
	{
		for (uint8 currPage64Idx = 0; currPage64Idx < SMARTALLOC_PAGES_PER_BLOCK / 64; currPage64Idx++)
		{
			if (allocMapBase.allocMapBase64[currBlockIdx][currPage64Idx] != 0xffffffffffffffff)
			{
				for (uint8 currPage8Idx = 0; currPage8Idx < 8; currPage8Idx++)
				{
					if (allocMapBase.allocMapBase8[currBlockIdx][currPage64Idx * 8 + currPage8Idx] != 0xff)
					{
						uint8 map = allocMapBase.allocMapBase8[currBlockIdx][currPage64Idx * 8 + currPage8Idx];
						for (uint8 displ = 0; displ < 8; displ++)
						{
							uint8 absPageIdx = currPage64Idx * 64 + currPage8Idx * 8 + displ;
							if ((!(map & 1)) && segmSizeBase[currBlockIdx][absPageIdx] >= pages)
							{
								SetMapBits(currBlockIdx, absPageIdx, pages + 1, true);
								if (segmSizeBase[currBlockIdx][absPageIdx] > pages)
									segmSizeBase[currBlockIdx][absPageIdx + pages + 1] = segmSizeBase[currBlockIdx][absPageIdx] - pages - 1;	//OVERFLOW!!!!!
								segmSizeBase[currBlockIdx][absPageIdx] = pages;
								return (void*) (((uint8*)allocBlockBase[currBlockIdx]) + absPageIdx * SMARTALLOC_PAGE_SIZE);
							}
							map >>= 1;
						}
					}
				}
			}
		}
	}

	if (allocatedBlockCount >= SMARTALLOC_BLOCK_COUNT)
		return nullptr;

	AllocateNewBlock();
	SetMapBits(allocatedBlockCount - 1, 0, pages + 1, true);
	segmSizeBase[allocatedBlockCount - 1][0] = pages;
	segmSizeBase[allocatedBlockCount - 1][pages + 1] = SMARTALLOC_PAGES_PER_BLOCK - pages - 2;
	return allocBlockBase[allocatedBlockCount - 1];
}

void SmartAllocator::Free(void* ptr)
{
	for (uint8 currBlockIdx = 0; currBlockIdx < allocatedBlockCount; currBlockIdx++)
	{
		if (ptr >= allocBlockBase[currBlockIdx] &&
			ptr < (uint8*)allocBlockBase[currBlockIdx] + SMARTALLOC_BLOCK_SIZE)
		{
			if (((uint8*)ptr - (uint8*)allocBlockBase[currBlockIdx]) % SMARTALLOC_PAGE_SIZE)
				throw;
			uint8 page = ((uint8*)ptr - (uint8*)allocBlockBase[currBlockIdx]) / SMARTALLOC_PAGE_SIZE;
			uint8 segmSize = segmSizeBase[currBlockIdx][page] + 1;
			bool nextFree = !GetMapBit(currBlockIdx, page + segmSize);
			if (page)
			{
				if (!GetMapBit(currBlockIdx, page - 1))
				{
					uint8 lastFreeSegmBeginIdx8 = page / 8;
					uint8 firstSetBitIdx = ReverseFindFirstSetBitInMap8(currBlockIdx, lastFreeSegmBeginIdx8, page % 8);
					if (firstSetBitIdx == UINT8_MAX)
					{
						firstSetBitIdx = 0;
						lastFreeSegmBeginIdx8 = 0;
						if (page / 8 >= 1)
						{
							for (int16 currPageIdx8 = page / 8 - 1; currPageIdx8 >= 0; currPageIdx8--)
							{
								if (allocMapBase.allocMapBase8[currBlockIdx][currPageIdx8])
								{
									firstSetBitIdx = ReverseFindFirstSetBitInMap8(currBlockIdx, (uint8)currPageIdx8);
									if (firstSetBitIdx == 7)
										currPageIdx8++;
									firstSetBitIdx = (firstSetBitIdx + 1) % 8;
									lastFreeSegmBeginIdx8 = (uint8)currPageIdx8;
									break;
								}
							}
						}
					}
					else
						firstSetBitIdx++;

					uint8 lastFreeSegmBegin = lastFreeSegmBeginIdx8 * 8 + firstSetBitIdx;
					segmSizeBase[currBlockIdx][lastFreeSegmBegin] += segmSize;
					segmSizeBase[currBlockIdx][page] = 0;
					if (nextFree)
					{
						segmSizeBase[currBlockIdx][lastFreeSegmBegin] += segmSizeBase[currBlockIdx][page + segmSize] + 1;
						segmSizeBase[currBlockIdx][page + segmSize] = 0;
					}
				}
				else
				{
					if (nextFree)
					{
						segmSizeBase[currBlockIdx][page] += segmSizeBase[currBlockIdx][page + segmSize] + 1;
						segmSizeBase[currBlockIdx][page + segmSize] = 0;
					}
				}
			}
			else
			{
				if (nextFree)
				{
					segmSizeBase[currBlockIdx][0] = segmSizeBase[currBlockIdx][segmSize] + segmSize + 1;
					segmSizeBase[currBlockIdx][segmSize] = 0;
				}
			}
			SetMapBits(currBlockIdx, page, segmSize, false);
		}
	}
}