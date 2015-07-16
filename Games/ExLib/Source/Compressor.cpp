#include <malloc.h>
#include <memory.h>

#include "Compressor.h"

struct CompressedBlock
{
	uint16 displacement;
	uint8 data;
	uint8 size;
};

void Compressor::Compress(const void *data, uint32 size, void **compressedData, uint32 *compressedDataSize)
{
	uint8 *input = (uint8*) data;

	uint32 blocksCount = 0, uncompressedDataSize = size;
	uint32 currentInputPos = 0;
	while (currentInputPos < size)
	{
		uint32 blockSize = 1;
		while (currentInputPos + blockSize < size && input[currentInputPos + blockSize] == input[currentInputPos])
			blockSize++;
		if (blockSize > sizeof(CompressedBlock))
		{
			blocksCount++;
			uncompressedDataSize -= blockSize;
		}
		currentInputPos += blockSize;
	}

	*compressedDataSize = sizeof(uint32) + uncompressedDataSize + sizeof(uint32) + sizeof(CompressedBlock) * blocksCount;
	*compressedData = malloc(*compressedDataSize);
	uint8 *output = (uint8*) *compressedData;
	*(uint32*) output = uncompressedDataSize;
	*(uint32*) (output + sizeof(uint32) + uncompressedDataSize) = blocksCount;

	uint8 *currentUncompressed = output + sizeof(uint32);
	CompressedBlock *blocks = (CompressedBlock*) (output + sizeof(uint32) + uncompressedDataSize + sizeof(uint32));
	uint32 currentBlock = 0, lastBlockPos = 0;
	currentInputPos = 0;
	while (currentInputPos < size)
	{
		uint8 blockSize = 1;
		while (currentInputPos + blockSize < size && input[currentInputPos + blockSize] == input[currentInputPos])
			blockSize++;
		if (blockSize > sizeof(CompressedBlock))
		{
			CompressedBlock &block = blocks[currentBlock];
			block.displacement = uint16(currentInputPos - lastBlockPos);
			block.data = input[currentInputPos];
			block.size = blockSize;

			memcpy(currentUncompressed, input + lastBlockPos, block.displacement);

			currentUncompressed += block.displacement;
			lastBlockPos = currentInputPos + blockSize;
			currentBlock++;
		}
		currentInputPos += blockSize;
	}
	memcpy(currentUncompressed, input + lastBlockPos, currentInputPos - lastBlockPos);
}

bool Compressor::Decompress(const void *data, uint32 compressedDataSize, void **decompressedData, uint32 *decompressedDataSize)
{
	uint8 *input = (uint8*) data;

	uint32 uncompressedDataSize = *(uint32*) input;
	if (sizeof(uint32) + uncompressedDataSize + sizeof(uint32) > compressedDataSize)
		return false;
	uint32 blocksCount = *(uint32*) (input + sizeof(uint32) + uncompressedDataSize);
	if (sizeof(uint32) + uncompressedDataSize + sizeof(uint32) + sizeof(CompressedBlock) * blocksCount != compressedDataSize)
		return false;
	CompressedBlock* blocks = (CompressedBlock*) (input + sizeof(uint32) + uncompressedDataSize + sizeof(uint32));

	*decompressedDataSize = uncompressedDataSize;
	uint32 uncompressedDataSizeCheck = 0;
	for (uint32 blockIdx = 0; blockIdx < blocksCount; blockIdx++)
	{
		*decompressedDataSize += blocks[blockIdx].size;
		uncompressedDataSizeCheck += blocks[blockIdx].displacement;
	}

	if (uncompressedDataSizeCheck > uncompressedDataSize)
		return false;

	*decompressedData = malloc(*decompressedDataSize);
	uint8 *currentOutput = (uint8*) *decompressedData;
	uint8 *currentUncompressed = input + sizeof(uint32);
	for (uint32 blockIdx = 0; blockIdx < blocksCount; blockIdx++)
	{
		CompressedBlock &block = blocks[blockIdx];
		memcpy(currentOutput, currentUncompressed, block.displacement);
		currentOutput += block.displacement;
		currentUncompressed += block.displacement;
		memset(currentOutput, block.data, block.size);
		currentOutput += block.size;
	}
	memcpy(currentOutput, currentUncompressed, uncompressedDataSize - uncompressedDataSizeCheck);

	return true;
}