#pragma once

#include "extypes.h"
#include "BitField.h"

class StaticListEnumerator
{
	template <typename Type, uint32 size> friend class StaticOrderedList;
private:
	uint32 currentIndex, lastIndex;
	StaticListEnumerator(uint32 _currentIndex, uint32 _lastIndex) : currentIndex(_currentIndex), lastIndex(_lastIndex) {}

public:
	StaticListEnumerator() : currentIndex(uint32(-1)), lastIndex(uint32(-1)) {}
};

template <typename Type, uint32 size>
class StaticOrderedList
{
private:
	struct ListNode
	{
		Type data;
		uint32 nextIndex;
	} buffer[size];
	BitField32<intdivceil_constexpr(size, 32)> allocBitField;
	uint32 firstNodeIdx;

public:
	inline StaticOrderedList()
	{
		firstNodeIdx = uint32(-1);
		allocBitField.Clear();
	}

	inline StaticListEnumerator GetFrontEnum()
	{
		return StaticListEnumerator(firstNodeIdx, uint32(-1));
	}
	inline void Enumerate(StaticListEnumerator& enumerator)
	{
		enumerator.lastIndex = enumerator.currentIndex;
		enumerator.currentIndex = buffer[enumerator.currentIndex].nextIndex;
	}
	inline Type& GetElement(const StaticListEnumerator& enumerator)
	{
		return buffer[enumerator.currentIndex].data;
	}

	inline bool Insert(const Type& element)
	{
		if (firstNodeIdx == uint32(-1))
		{
			allocBitField.SetBitTo1(0);
			buffer[0].data = element;
			buffer[0].nextIndex = uint32(-1);
			firstNodeIdx = 0;
		}
		else
		{
			uint32 newIndex = allocBitField.FindFirst0();
			if (newIndex >= size)
				return false;
			allocBitField.SetBitTo1(newIndex);
			buffer[newIndex].data = element;

			if (buffer[firstNodeIdx].data > element)
			{
				buffer[newIndex].nextIndex = firstNodeIdx;
				firstNodeIdx = newIndex;
				return true;
			}

			uint32 lastIndex = firstNodeIdx;
			uint32 currentIndex = buffer[firstNodeIdx].nextIndex;
			while (currentIndex != uint32(-1))
			{
				if (buffer[currentIndex].data > element)
					break;
				lastIndex = currentIndex;
				currentIndex = buffer[currentIndex].nextIndex;
			}
			buffer[newIndex].nextIndex = currentIndex;
			buffer[lastIndex].nextIndex = newIndex;
		}
		return true;
	}
	inline void Delete(StaticListEnumerator& enumerator)
	{
		allocBitField.SetBitTo0(enumerator.currentIndex);
		if (enumerator.lastIndex == uint32(-1))
			firstNodeIdx = buffer[enumerator.currentIndex].nextIndex;
		else
			buffer[enumerator.lastIndex].nextIndex = buffer[enumerator.currentIndex].nextIndex;
		enumerator.currentIndex = buffer[enumerator.currentIndex].nextIndex;
	}

	inline bool IsEmpty()
	{
		return firstNodeIdx == uint32(-1);
	}
	inline bool EnumIsValid(const StaticListEnumerator& enumerator)
	{
		return enumerator.currentIndex < size;
	}
};