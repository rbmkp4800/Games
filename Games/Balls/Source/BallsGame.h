#include <extypes.h>
#include <extypes.vectors.h>
#include <Render2D.h>

namespace BallsGame
{
	struct PlayerBall
	{
		float32x2 position, speed, acceleration;
		float radius, charge;
	};

	class GameObject abstract
	{
	protected:
		float32x2 position;

	public:
		inline float32x2 GetPosition() { return position; }
		inline void Move(float posDelta) { position.y += posDelta; }
	};

	class StaticBall : public GameObject
	{
	private:
		float radius, charge;

	public:
		void CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& playerBallTranslation);
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall);

		void DrawBackground();
		void DrawForeground();
	};

	class Field
	{
	private:
		static const uint32 staticBallsLimit = 64;

		StaticBall staticBalls[staticBallsLimit];
		uint32 staticBallsCount;

	public:
		void Clear();
		void InteractPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2 playerBallTranslation);
		void UpdateAndDraw(float posDelta, Render2D::Batch* batch);
	};

	class Background
	{
	private:
		static const uint32 blursLimit = 32;

		Render2D::VertexEllipse blursVertexBuffer[blursLimit * 8];
		struct blurDesc
		{
			float posDeltaCoef;
		} blurDescs[blursLimit];
		uint32 blursCount;
		rectf32 rect;

	public:
		inline Background() : blursCount(0), rect(0.0f, 0.0f, 0.0f, 0.0f) {}
		void Generate();
		void UpdateAndDraw(float posDelta, Render2D::Batch* batch);
	};

	enum class PlayerControl
	{
		None = 0,
		Left = 1,
		Right = 2,
		Up = 3,
		Down = 4,
	};

	class Game
	{
	private:
		static Render2D::Device device;

		Render2D::SwapChain swapChain;

		PlayerBall playerBall;
		Field field;
		Background background;

	public:
		bool Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY);
		void Update(float timeDelta);

		void SetPlayerControlState(PlayerControl playerControl, bool state);
		void ResizeOutput(uint32 x, uint32 y);
		void Restart();
	};
}