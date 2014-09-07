#include <Render2D.h>

#define FLAG_UP		0x01
#define FLAG_DOWN	0x02
#define FLAG_LEFT	0x04
#define FLAG_RIGHT	0x08
#define FLAG_DROPBOMB	0x10

#define BOMBEREXCEP_WRONG_COORDS		1
#define BOMBEREXCEP_OBJ_ALREDY_EXIST	2
#define BOMBEREXCEP_OVERFLOW			3

#define GetTextureTileRect(tileCount, currentTile) rectf32((float)(currentTile) / (float)(tileCount), 0.0f,	\
		(float)((currentTile) + 1) / (float)(tileCount), 1.0f)

#include "Bomberman.SmartAllocator.h"
#include "Bomberman.GameObjects.h"
#include "Bomberman.Characters.h"
#include "Bomberman.Field.h"

namespace Bomberman
{
	class ControlsState
	{
	private:
		uint8 flags;

	public:
		inline ControlsState() : flags(0) {}
		inline bool Up() { return flags & FLAG_UP ? true : false; }
		inline bool Down() { return flags & FLAG_DOWN ? true : false; }
		inline bool Left() { return flags & FLAG_LEFT ? true : false; }
		inline bool Right() { return flags & FLAG_RIGHT ? true : false; }
		inline bool DropBomb() { return flags & FLAG_DROPBOMB ? true : false; }
		inline void Up(bool state) { if (state) flags |= FLAG_UP; else flags &= ~FLAG_UP; }
		inline void Down(bool state) { if (state) flags |= FLAG_DOWN; else flags &= ~FLAG_DOWN; }
		inline void Left(bool state) { if (state) flags |= FLAG_LEFT; else flags &= ~FLAG_LEFT; }
		inline void Right(bool state) { if (state) flags |= FLAG_RIGHT; else flags &= ~FLAG_RIGHT; }
		inline void DropBomb(bool state) { if (state) flags |= FLAG_DROPBOMB; else flags &= ~FLAG_DROPBOMB; }
	};

	class GameBase abstract
	{
	public:
		static Player *player;
		static ControlsState playerControlsState;
		static GameObjectsControllerNoDepth goController0, goController2;
		static GameObjectsController goController1, goController3;
		static void Init();
	};

	class Core abstract
	{
	public:
		static bool Load();

		static void SetSwapChain(Render2D::SwapChain* _swapChain);
		static void OnSwapChainResize();
		static bool blurEnabled;

		static bool Update();
	};

	class ExplosionsController abstract
	{
	public:
		static void Clear();
		static bool Update(uint16 delta);
		static void Create(uint16 x, uint16 y, uint8 power);
		static void DrawUnderBlocksPart();
		static void DrawOverBlocksPart();
	};

	class Camera abstract
	{
	public:
		static void Clear();
		static void ChangeOutputSize(uint32x2 outputSize);
		static void SetFocusPoint(float32x2 point);
		static void Shake();
		static bool Update(uint16 delta);
		static void ChangeScale(float targetScale, float pitch);
		static void SetAdditionalTransform(matrix3x2& transform);
		static void ResetTransform();
		//static Render2D::Matrix GetTransform();
	};

	class GameTimer abstract
	{
	public:
		static void Init();
		static uint16 Update();
		static void ChangeSpeed(float target, float pitch);
	};

	class Resources
	{
	public:
		Render2D::Texture
			wallTexture, boxTexture, brickWallTexture,
			floorBackTexture, floorForeStyle1Texture, floorForeStyle2Texture,
			bombTexture,
			flameTexture, explosionCenterTexture, explosionWaveTexture,
			brickDestructionTexture, boxDestructionTexture, floorExplosionMarkTexture, dustTexture,
			playerStandTexture, playerDownTexture, playerUpTexture, playerLeftTexture, playerRightTexture,
			ghostTexture, jockStandTexture, jockUpTexture, jockDownTexture;
		bool Load();
	};
	extern Resources resources;
}