#pragma once

#include "extypes.h"

class Compressor abstract final
{
public:
	static void Compress(const void* data, uint32 size, void** comressedData, uint32* comressedDataSize);
	static bool Decompress(const void* data, uint32 size, void** decompressedData, uint32* decompressedDataSize);
};