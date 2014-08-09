#include <malloc.h>
#include <memory.h>

#include "BallsGame.h"

GameObjectsController::GameObjectsController() : firstObjectIdx(0), lastObjectIdx(0), lastObjectSize(0)
{
	memBuffer = malloc(bufferSize);
	memset(objectsBuffer, 0, sizeof(GameObject*) * objectsLimit);
	memset(memBuffer, 0, bufferSize);
}
GameObjectsController::~GameObjectsController()
{
	free(memBuffer);
}

inline void GameObjectsController::deleteFirstObject()
{
	if (objectsBuffer[firstObjectIdx])
	{
		objectsBuffer[firstObjectIdx]->~GameObject();
		objectsBuffer[firstObjectIdx] = nullptr;
		firstObjectIdx++;
		firstObjectIdx %= objectsLimit;
	}
}
void* GameObjectsController::allocate(uint32 _objectSize)
{
	uint32 objectSize = _objectSize;
	if (objectSize > bufferSize)
		throw "Cyclic game objects controller: too large object";

	if (objectsBuffer[firstObjectIdx])
	{
		if ((lastObjectIdx + 1) % objectsLimit == firstObjectIdx)
			deleteFirstObject();

		uintptr newLastObjectPtr = uintptr(objectsBuffer[lastObjectIdx]) + lastObjectSize;
		if (uintptr(objectsBuffer[firstObjectIdx]) <= newLastObjectPtr)
		{
			if (newLastObjectPtr + objectSize > uintptr(memBuffer) + bufferSize)
			{
				newLastObjectPtr = uintptr(memBuffer);
				while (newLastObjectPtr + objectSize > uintptr(objectsBuffer[firstObjectIdx]) && objectsBuffer[firstObjectIdx])
					deleteFirstObject();
			}
		}
		else
		{
			if (newLastObjectPtr + objectSize > uintptr(memBuffer) + bufferSize)
			{
				newLastObjectPtr = uintptr(memBuffer);
				uintptr tmpFirstObjectPtr = uintptr(objectsBuffer[firstObjectIdx]);
				while (uintptr(objectsBuffer[firstObjectIdx]) >= tmpFirstObjectPtr)
					deleteFirstObject();
			}
			while (newLastObjectPtr + objectSize > uintptr(objectsBuffer[firstObjectIdx]) && objectsBuffer[firstObjectIdx])
				deleteFirstObject();
		}

		lastObjectIdx++;
		lastObjectIdx %= objectsLimit;
		objectsBuffer[lastObjectIdx] = (GameObject*) newLastObjectPtr;
	}
	else
	{
		firstObjectIdx = 0;
		lastObjectIdx = 0;
		objectsBuffer[0] = (GameObject*) memBuffer;
	}
	lastObjectSize = objectSize;

	return objectsBuffer[lastObjectIdx];
}
void GameObjectsController::Dispatch(GameObject::Message message)
{
	if (!objectsBuffer[firstObjectIdx])
		return;
	if (firstObjectIdx <= lastObjectIdx)
	{
		for (uint16 i = firstObjectIdx; i <= lastObjectIdx; i++)
			objectsBuffer[i]->Dispatch(message);
	}
	else
	{
		for (uint16 i = firstObjectIdx; i < objectsLimit; i++)
			objectsBuffer[i]->Dispatch(message);
		for (uint16 i = 0; i <= lastObjectIdx; i++)
			objectsBuffer[i]->Dispatch(message);
	}
}
void GameObjectsController::Clear()
{
	if (objectsBuffer[firstObjectIdx])
	{
		if (firstObjectIdx <= lastObjectIdx)
		{
			for (uint16 i = firstObjectIdx; i <= lastObjectIdx; i++)
				objectsBuffer[i]->~GameObject();
		}
		else
		{
			for (uint16 i = firstObjectIdx; i < objectsLimit; i++)
				objectsBuffer[i]->~GameObject();
			for (uint16 i = 0; i <= lastObjectIdx; i++)
				objectsBuffer[i]->~GameObject();
		}
	}
	memset(objectsBuffer, 0, sizeof(GameObject*) * objectsLimit);
	firstObjectIdx = 0;
	lastObjectIdx = 0;
}