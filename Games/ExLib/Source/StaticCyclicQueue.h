#pragma once

#include "extypes.h"

template <typename Type, uintptr size>
class StaticCyclicQueue
{
private:
	Type buffer[size];
	uintptr frontIdx, backIdx;

public:
	inline StaticCyclicQueue() : frontIdx(0), backIdx(0) {}

	inline void PushBack(const Type& value)
	{
		buffer[backIdx] = value;
		backIdx = (backIdx + 1) % size;
	}
	inline void PushFront(const Type& value)
	{
		frontIdx = (frontIdx + size - 1) % size;
		buffer[frontIdx] = value;
	}
	inline void PopFront()
	{
		frontIdx = (frontIdx + 1) % size;
	}
	inline void PopBack()
	{
		backIdx = (backIdx + size - 1) % size;
	}

	inline Type& PeekFront() { return buffer[frontIdx]; }
	inline Type& PeekBack() { return buffer[(backIdx + size - 1) % size]; }
	inline Type& operator [](uintptr index) { return buffer[(frontIdx + index) % size]; }

	inline uint32 GetElementsCount() { return (backIdx + size - frontIdx) % size; }
	inline bool IsEmpty() { return frontIdx == backIdx; }
	inline bool IsFull() { return frontIdx == ((backIdx + 1) % size); }

	inline void Clear() { frontIdx = 0; backIdx = 0; }
};