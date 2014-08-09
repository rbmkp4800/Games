#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

#define SHAKE_FREQUENCY			8.0f
#define SHAKE_AMPLITUDE			0.4f

#define SHAKE_TIMER			1000

#define MAX_SHAKE_COUNT		8

#define SCREEN_PART_X_FOCUS_ZONE	0.2f
#define SCREEN_PART_Y_FOCUS_ZONE	0.1f

static matrix3x2 basicTransform;

static uint16 shakeTicksBase[MAX_SHAKE_COUNT];
static uint32 shakeCount;

static float scale;
uint32x2 outputSize;

static float32x2 screenSizeFieldDim, screenPosFieldDim, focusPointFieldDim;

static float targetBlocksPerScreenX, scaleChangePitch;

void Camera::Clear()
{
	shakeCount = 0;
	targetBlocksPerScreenX = 16.0f;

	screenPosFieldDim.set(0.0f, 0.0f);
	focusPointFieldDim.set(0.0f, 0.0f);
}

void Camera::ChangeOutputSize(uint32x2 _outputSize)
{
	outputSize = _outputSize;
	screenSizeFieldDim.x = targetBlocksPerScreenX;
	screenSizeFieldDim.y = screenSizeFieldDim.x / outputSize.aspect();
	scale = outputSize.x / screenSizeFieldDim.x;

	basicTransform = matrix3x2::scale(scale, scale)
		* matrix3x2::translation(-screenPosFieldDim.x, -screenPosFieldDim.y);
}

void Camera::ChangeScale(float targetScale, float pitch)
{
	targetBlocksPerScreenX = targetScale;
	scaleChangePitch = pitch;
}

void Camera::Shake()
{
	if (shakeCount < MAX_SHAKE_COUNT)
		shakeTicksBase[shakeCount++] = 0;
}

bool Camera::Update(uint16 delta)
{
	bool needMatrixUpdate = false;

	if (scaleChangePitch != 0.0f)
	{
		if (targetBlocksPerScreenX > screenSizeFieldDim.x)
		{
			screenSizeFieldDim.x += scaleChangePitch;
			if (targetBlocksPerScreenX <= screenSizeFieldDim.x)
			{
				scaleChangePitch = 0.0f;
				screenSizeFieldDim.x = targetBlocksPerScreenX;
			}
		}
		if (targetBlocksPerScreenX < screenSizeFieldDim.x)
		{
			screenSizeFieldDim.x -= scaleChangePitch;
			if (targetBlocksPerScreenX >= screenSizeFieldDim.x)
			{
				scaleChangePitch = 0.0f;
				screenSizeFieldDim.x = targetBlocksPerScreenX;
			}
		}
		if (screenSizeFieldDim.x == targetBlocksPerScreenX)
			scaleChangePitch = 0.0f;

		screenSizeFieldDim.y = screenSizeFieldDim.x / outputSize.aspect();
		scale = outputSize.x / screenSizeFieldDim.x;
		needMatrixUpdate = true;
	}

	if (screenPosFieldDim.x + screenSizeFieldDim.x * (0.5f - SCREEN_PART_X_FOCUS_ZONE) - focusPointFieldDim.x > 0.05f)
	{
		screenPosFieldDim.x += (focusPointFieldDim.x - (screenPosFieldDim.x + screenSizeFieldDim.x * (0.5f - SCREEN_PART_X_FOCUS_ZONE))) * 0.05f;
		needMatrixUpdate = true;
	}
	if (focusPointFieldDim.x - (screenPosFieldDim.x + screenSizeFieldDim.x * (0.5f + SCREEN_PART_X_FOCUS_ZONE)) > 0.05f)
	{
		screenPosFieldDim.x += (focusPointFieldDim.x - (screenPosFieldDim.x + screenSizeFieldDim.x * (0.5f + SCREEN_PART_X_FOCUS_ZONE))) * 0.05f;
		needMatrixUpdate = true;
	}
	if (screenPosFieldDim.y + screenSizeFieldDim.y * (0.5f - SCREEN_PART_Y_FOCUS_ZONE) - focusPointFieldDim.y > 0.05f)
	{
		screenPosFieldDim.y += (focusPointFieldDim.y - (screenPosFieldDim.y + screenSizeFieldDim.y * (0.5f - SCREEN_PART_Y_FOCUS_ZONE))) * 0.05f;
		needMatrixUpdate = true;
	}
	if (focusPointFieldDim.y - (screenPosFieldDim.y + screenSizeFieldDim.y * (0.5f + SCREEN_PART_Y_FOCUS_ZONE)) > 0.05f)
	{
		screenPosFieldDim.y += (focusPointFieldDim.y - (screenPosFieldDim.y + screenSizeFieldDim.y * (0.5f + SCREEN_PART_Y_FOCUS_ZONE))) * 0.05f;
		needMatrixUpdate = true;
	}

	if (!(shakeCount || needMatrixUpdate))
		return false;

	float resultTranslation = 0.0f;
	for (int16 currShake = shakeCount - 1; currShake >= 0; currShake--)
	{
		float tmp = (1.0f - shakeTicksBase[currShake] / (float)SHAKE_TIMER);
		resultTranslation += sinf(shakeTicksBase[currShake] * SHAKE_FREQUENCY * PI / 1000.0f) * tmp * tmp;
		shakeTicksBase[currShake] += delta;

		if (shakeTicksBase[currShake] > SHAKE_TIMER)
			shakeTicksBase[currShake] = shakeTicksBase[--shakeCount];
	}
	resultTranslation *= SHAKE_AMPLITUDE * scale;
	resultTranslation = (float)(int)resultTranslation;
	resultTranslation /= scale;

	basicTransform = matrix3x2::rotation(outputSize.x / 2.0f, outputSize.y / 2.0f, resultTranslation / 5.0f)
		* matrix3x2::scale(scale, scale) * matrix3x2::translation(-screenPosFieldDim.x, -screenPosFieldDim.y);

	return true;
}

void Camera::SetFocusPoint(float32x2 point)
{
	focusPointFieldDim = point;
}

void Camera::SetAdditionalTransform(matrix3x2& transform)
{
	Render::SetTransform(basicTransform * transform);
}
void Camera::ResetTransform()
{
	Render::SetTransform(basicTransform);
}
/*Matrix Camera::GetTransform()
{
	return basicTransform;
}*/