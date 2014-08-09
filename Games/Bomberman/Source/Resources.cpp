#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

Bomberman::Resources Bomberman::resources;

bool Resources::Load()
{
	wallTexture.CreateFromPng(L"Textures\\wall.png");
	boxTexture.CreateFromPng(L"Textures\\box.png");
	brickWallTexture.CreateFromPng(L"Textures\\brickWall.png");

	floorBackTexture.CreateFromPng(L"Textures\\floorBack.png");
	floorForeStyle1Texture.CreateFromPng(L"Textures\\floorForeStyle1.png");
	floorForeStyle2Texture.CreateFromPng(L"Textures\\floorForeStyle2.png");

	bombTexture.CreateFromPng(L"Textures\\bomb.png");

	flameTexture.CreateFromPng(L"Textures\\flames.png");
	explosionCenterTexture.CreateFromPng(L"Textures\\explosion.png");
	explosionWaveTexture.CreateFromPng(L"Textures\\explosionWave.png");
	floorExplosionMarkTexture.CreateFromPng(L"Textures\\floorExplosionMark.png");

	dustTexture.CreateFromPng(L"Textures\\dust.png");
	brickDestructionTexture.CreateFromPng(L"Textures\\brickDestruction.png");
	boxDestructionTexture.CreateFromPng(L"Textures\\boxDestruction.png");

	playerStandTexture.CreateFromPng(L"Textures\\playerStand.png");
	playerDownTexture.CreateFromPng(L"Textures\\playerDown.png");
	playerUpTexture.CreateFromPng(L"Textures\\playerUp.png");
	playerLeftTexture.CreateFromPng(L"Textures\\playerLeft.png");
	playerRightTexture.CreateFromPng(L"Textures\\playerRight.png");

	ghostTexture.CreateFromPng(L"Textures\\ghost.png");
	jockStandTexture.CreateFromPng(L"Textures\\jockStand.png");
	jockUpTexture.CreateFromPng(L"Textures\\jockUp.png");
	jockDownTexture.CreateFromPng(L"Textures\\jockDown.png");

	return true;
}