#pragma once

#include "extypes.h"

template <uint32 size32>
class BitField32
{
private:
	uint32 buffer[size32];

public:
	inline void Clear()
	{
		for (uint32 i = 0; i < size32; i++)
			buffer[i] = 0;
	}
	inline uint32 FindFirst0()
	{
		for (uint32 i = 0; i < size32; i++)
		{
			uint32 val = buffer[i];
			if (val != uint32(-1))
			{
				i *= 32;
				if (uint16(val) == uint16(-1))
				{
					val >>= 16;
					i += 16;
				}
				if (uint8(val) == uint8(-1))
				{
					val >>= 8;
					i += 8;
				}
				for (uint8 j = 0; j < 8; j++)
				{
					if (val & 1 == 0)
						return i + j;
					val >>= 1;
				}
				return uint32(-1);
			}
		}
		return uint32(-1);
	}
	inline void SetBitTo1(uint32 idx)
	{
		buffer[idx / 32] |= 1 << (idx % 32);
	}
	inline void SetBitTo0(uint32 idx)
	{
		buffer[idx / 32] &= ~(1 << (idx % 32));
	}
};