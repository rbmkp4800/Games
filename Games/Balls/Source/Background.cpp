#include "BallsGame.h"

#include <stdlib.h>

using namespace BallsGame;
using namespace Render2D;

static const float maxNewBlurDelta = 0.1f;

void Background::Generate()
{

}

void Background::UpdateAndDraw(float posDelta, Batch* batch)
{
	batch->GetDevice()->Clear(colors::darkMagenta);
}