#include "BallsGame.h"

using namespace Render2D;

Texture Textures::ballMask, Textures::interactionPushSprite,
	Textures::interactionPullSprite, Textures::backgroundMask;

bool Textures::Load()
{
	bool result = true;
	result &= ballMask.CreateFromPng(L"Textures\\ballMask.png");
	result &= interactionPushSprite.CreateFromPng(L"Textures\\redWave.png");
	result &= interactionPullSprite.CreateFromPng(L"Textures\\blueWave.png");
	result &= backgroundMask.CreateFromPng(L"Textures\\bgMask.png");
	return result;
}