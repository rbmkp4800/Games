#include <ppltasks.h>

#include "Bomberman.h"

using namespace Windows::Foundation;

using namespace Bomberman;
using namespace Render2D;

Render2D::SwapChain swapChain;
Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainPanel;

namespace BombermanLib
{
	public ref class GameControls sealed
	{
	public:
		static property bool Up
		{
			bool get() { return GameBase::playerControlsState.Up(); }
			void set(bool val) { GameBase::playerControlsState.Up(val); }
		}
		static property bool Down
		{
			bool get() { return GameBase::playerControlsState.Down(); }
			void set(bool val) { GameBase::playerControlsState.Down(val); }
		}
		static property bool Left
		{
			bool get() { return GameBase::playerControlsState.Left(); }
			void set(bool val) { GameBase::playerControlsState.Left(val); }
		}
		static property bool Right
		{
			bool get() { return GameBase::playerControlsState.Right(); }
			void set(bool val) { GameBase::playerControlsState.Right(val); }
		}
		static bool DropBomb()
		{
			if (GameBase::playerControlsState.DropBomb())
				return false;
			GameBase::playerControlsState.DropBomb(true);
			return true;
		}
	};

	public ref class GameInterface sealed
	{
	public:
		static void Load()
		{
			Core::Load();
		}
		static void SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ panel)
		{
			swapChainPanel = panel;
			swapChain = SwapChain((IUnknown*)swapChainPanel, (uint16)swapChainPanel->ActualWidth, (uint16)swapChainPanel->ActualHeight);
			Core::SetSwapChain(&swapChain);
		}
		static void ResizeSwapChain(uint32 sizeX, uint32 sizeY)
		{
			if (swapChain.Resize(sizeX, sizeY))
				Core::OnSwapChainResize();
		}
		static bool Update()
		{
			return Core::Update();
		}
		static IAsyncAction^ Present()
		{
			return concurrency::create_async([]()
			{
				swapChain.Present();
			});
		}
		static void ChangeSpeedOfTime(float target, float pitch)
		{
			GameTimer::ChangeSpeed(target, pitch);
		}
		static void ChangeScale(float target, float pitch)
		{
			Camera::ChangeScale(target, pitch);
		}
		static property uint32 RedrawCount
		{
			uint32 get(){ return 0; }
		}
		static property bool BlurEnabled
		{
			bool get() { return Core::blurEnabled; }
			void set(bool val) { Core::blurEnabled = val; }
		}
	};
}