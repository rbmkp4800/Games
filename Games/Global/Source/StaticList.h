#pragma once

#include "extypes.h"
#include "BitField.h"

template <typename Type, uint32 size>
class StaticList
{
private:
	struct ListNode
	{
		Type data;
		uint32 nextIdx;
	} buffer[size];
	uint32 firstNodeIdx;
	BitField32<size / 32 + (size % 32) ? 1 : 0> allocBitField;

public:
	StaticList()
	{
		firstNodeIdx = uint32(-1);
		allocBitField.Clear();
	}
	void PushAfter()
};