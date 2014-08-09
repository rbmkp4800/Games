#include <string.h>

#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

#define GO_ALREDY_UPDATED_FLAG	0x8000

//---------------------------------GameObjectsController--------------------------------//

bool GameObjectsController::Update(uint16 delta)
{
	if (!goCount)
		return false;
	
	bool needRedraw = objectsAdded;
	objectsAdded = false;

	for (int16 currObjIdx = 0; currObjIdx < goCount; currObjIdx++)
	{
		if (goDrawPriorityBase[currObjIdx] & GO_ALREDY_UPDATED_FLAG)
		{
			goDrawPriorityBase[currObjIdx] &= ~GO_ALREDY_UPDATED_FLAG;
			continue;
		}
		goDrawPriorityBase[currObjIdx] &= ~GO_ALREDY_UPDATED_FLAG;

		GOUpdateResult result = goBase[currObjIdx]->Update(delta);

		switch (result.type)
		{
		case GOUpdateType::PositionChanged:
			needRedraw = true;
			{
				int16 newIdx = currObjIdx;
				uint16 newPriority = result.drawPriority;
				if (newPriority > goDrawPriorityBase[currObjIdx])
				{
					while (newIdx < goCount - 1 && newPriority > goDrawPriorityBase[newIdx + 1]) newIdx++;
					if (uint32 elementsToMove = newIdx - currObjIdx)
					{
						IGameObject *currObject = goBase[currObjIdx];
						memmove(goBase + currObjIdx, goBase + currObjIdx + 1, sizeof(IGameObject*) * elementsToMove);
						memmove(goDrawPriorityBase + currObjIdx, goDrawPriorityBase + currObjIdx + 1, sizeof(uint16) * elementsToMove);
						goBase[newIdx] = currObject;
						goDrawPriorityBase[newIdx] = newPriority | GO_ALREDY_UPDATED_FLAG;
						currObjIdx--;
					}
					else
						goDrawPriorityBase[currObjIdx] = newPriority;
				}
				else if (newPriority < goDrawPriorityBase[currObjIdx])
				{
					while (newIdx && newPriority < goDrawPriorityBase[newIdx - 1]) newIdx--;
					if (uint32 elementsToMove = currObjIdx - newIdx)
					{
						IGameObject *currObject = goBase[currObjIdx];
						memmove(goBase + newIdx + 1, goBase + newIdx, sizeof(IGameObject*) * elementsToMove);
						memmove(goDrawPriorityBase + newIdx + 1, goDrawPriorityBase + newIdx, sizeof(uint16) * elementsToMove);
						goBase[newIdx] = currObject;
					}
					goDrawPriorityBase[newIdx] = newPriority;
				}
			}
			break;

		case GOUpdateType::Release:
			needRedraw = true;
			delete goBase[currObjIdx];
			if (uint32 elementsToMove = goCount - currObjIdx - 1)
			{
				memmove(goBase + currObjIdx, goBase + currObjIdx + 1, sizeof(IGameObject*) * elementsToMove);
				memmove(goDrawPriorityBase + currObjIdx, goDrawPriorityBase + currObjIdx + 1, sizeof(uint16) * elementsToMove);
				currObjIdx--;
			}
			goCount--;
			break;

		case GOUpdateType::Redraw:
			needRedraw = true;
			break;
		}
	}

	return needRedraw;
}

void GameObjectsController::Draw()
{
	for (int16 currObjIdx = 0; currObjIdx < goCount; currObjIdx++)
		goBase[currObjIdx]->Draw();
}

void GameObjectsController::Add(IGameObject* object)
{
	if (goCount >= MAX_GOCOUNT_PER_CONTROLLER)
	{
		delete object;
		return;
	}

	uint16 objectPriority = object->Update(0).drawPriority;
	if (!goCount)
	{
		goBase[0] = object;
		goDrawPriorityBase[0] = objectPriority;
		goCount++;
		return;
	}
	if (objectPriority < goDrawPriorityBase[0])
	{
		memmove(goBase + 1, goBase, sizeof(IGameObject*) * goCount);
		memmove(goDrawPriorityBase + 1, goDrawPriorityBase, sizeof(uint16) * goCount);
		goBase[0] = object;
		goDrawPriorityBase[0] = objectPriority;
		goCount++;
		return;
	}
	if (objectPriority > goDrawPriorityBase[goCount - 1])
	{
		goBase[goCount] = object;
		goDrawPriorityBase[goCount] = objectPriority;
		goCount++;
		return;
	}

	uint32 firstObjIdx = 0, lastObjIdx = goCount;
	while (firstObjIdx < lastObjIdx)
	{
		uint32 middleObjIdx = (firstObjIdx + lastObjIdx) / 2;
		if (objectPriority < goDrawPriorityBase[middleObjIdx])
			lastObjIdx = middleObjIdx;
		else
			firstObjIdx = middleObjIdx + 1;
	}

	memmove(goBase + firstObjIdx + 1, goBase + firstObjIdx, sizeof(IGameObject*) * (goCount - firstObjIdx));
	memmove(goDrawPriorityBase + firstObjIdx + 1, goDrawPriorityBase + firstObjIdx, sizeof(uint16) * (goCount - firstObjIdx));
	goBase[firstObjIdx] = object;
	goDrawPriorityBase[firstObjIdx] = objectPriority;

	goCount++;
	objectsAdded = true;
}

//-----------------------------GameObjectsControllerNoDepth--------------------------------//

bool GameObjectsControllerNoDepth::Update(uint16 delta)
{
	bool needRedraw = false;

	for (int16 currObjIdx = 0; currObjIdx < goCount; currObjIdx++)
	{
		switch (goBase[currObjIdx]->Update(delta).type)
		{
		case GOUpdateType::PositionChanged:
		case GOUpdateType::Redraw:
			needRedraw = true;
			break;

		case GOUpdateType::Release:
			delete goBase[currObjIdx];
			goBase[currObjIdx--] = goBase[--goCount];
			needRedraw = true;
			break;
		}
	}

	return needRedraw;
}

void GameObjectsControllerNoDepth::Draw()
{
	for (int16 currObjIdx = 0; currObjIdx < goCount; currObjIdx++)
		goBase[currObjIdx]->Draw();
}

void GameObjectsControllerNoDepth::Add(IGameObject* object)
{
	if (goCount >= MAX_GOCOUNT_PER_CONTROLLER)
	{
		delete object;
		return;
	}

	goBase[goCount++] = object;
	objectsAdded = true;
}