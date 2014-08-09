#include <Windows.h>

#include "Bomberman.h"

using namespace Bomberman;
using namespace Render2D;

static SwapChain *swapChain;

bool Core::blurEnabled;
static RenderTarget blurRenderTarget1, blurRenderTarget2;

bool Core::Load()
{
	SmartAllocator::Init();

	resources.Load();

	Camera::Clear();
	GameTimer::Init();
	GameBase::Init();

	blurEnabled = false;

	return true;
}

bool Core::Update()
{
	uint16 delta = GameTimer::Update();

	bool redraw = false;
	redraw |= ExplosionsController::Update(delta);
	redraw |= GameBase::goController0.Update(delta);
	redraw |= GameBase::goController1.Update(delta);
	redraw |= GameBase::goController2.Update(delta);
	redraw |= GameBase::goController3.Update(delta);
	redraw |= Camera::Update(delta);

	if (redraw)
	{
		Field::current->Update();

		Render::SetTarget(swapChain);
		Render::Clear(colors::darkBlue);
		Camera::ResetTransform();

		Field::current->DrawFloor();
		GameBase::goController0.Draw();
		ExplosionsController::DrawUnderBlocksPart();
		Field::current->DrawBlocks();
		ExplosionsController::DrawOverBlocksPart();
		GameBase::goController1.Draw();
		GameBase::goController2.Draw();
		GameBase::goController3.Draw();
		Render::Flush();

		/*if (_blurEnabled)
		{
			//Device::SetTarget(swapChain);
			//Device::DrawBitmap(&blurRenderTarget1, &rectf32(0.0f, 0.0f, swapChain->GetSize().x, swapChain->GetSize().y));

			Device::SetTarget(&blurRenderTarget2);
			Device::DrawBitmapWithBlur(&blurRenderTarget1, Vertical, &rectf32(0.0f, 0.0f, blurRenderTarget2.GetSize().x, blurRenderTarget2.GetSize().y));
			Device::SetTarget(swapChain);
			Device::SetTransform(Matrix::Identity());
			Device::DrawBitmapWithBlur(&blurRenderTarget2, Horizontal, &rectf32(0.0f, 0.0f, swapChain->GetSize().x, swapChain->GetSize().y));
		}*/
	}

	return redraw;
}

void Core::SetSwapChain(Render2D::SwapChain* _swapChain)
{
	swapChain = _swapChain;
	uint32x2 size = swapChain->GetSize();
	Camera::ChangeOutputSize(size);

	size.x /= 2;
	size.y /= 2;
	/*blurRenderTarget1 = RenderTarget(size.x, size.y);
	blurRenderTarget2 = RenderTarget(size.x, size.y);*/
}
void Core::OnSwapChainResize()
{
	Camera::ChangeOutputSize(swapChain->GetSize());
}