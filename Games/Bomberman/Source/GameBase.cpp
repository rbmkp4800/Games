#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

ControlsState GameBase::playerControlsState;
Player *GameBase::player;

GameObjectsControllerNoDepth GameBase::goController0, GameBase::goController2;
GameObjectsController GameBase::goController1, GameBase::goController3;

void GameBase::Init()
{
	ExplosionsController::Clear();

	Field::current = new Bomberman::Field(60, 60);

	IMonster::Init();
	player = new Player(0, 0);
	goController1.Add(new Jock(10, 5));
	//goController1.Add(new Jock(11, 5));
	goController1.Add(new Ghost(5, 5));
	goController1.Add(player);
}