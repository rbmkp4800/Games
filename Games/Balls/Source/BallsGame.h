#include <extypes.h>
#include <extypes.vectors.h>

#include <Render2D.h>

namespace BallsGame
{
	struct PlayerBall
	{
		float32x2 position, speed;
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
		StaticBall() = default;
		inline StaticBall(float x, float y, float _radius, float _charge) : radius(_radius), charge(_charge) { position.set(x, y); }

		void CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& translation);
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall);
		void Draw(Render2D::Batch* batch);
	};

	class Field
	{
	private:
		static const uint32 staticBallsLimit = 64;

		StaticBall staticBalls[staticBallsLimit];
		uint32 staticBallsCount;

	public:
		void Clear();
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall);
		void CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& translation);
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
		Field field;
		Background background;
		PlayerBall playerBall;
		struct
		{
			bool left, right, up, down;
		} controls;
		float aspect, cameraDelta;
		float64 globalPosition;

		static Render2D::Device device;
		Render2D::SwapChain swapChain;
		uint32x2 outputSize;

	public:
		bool Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY);
		void Update(float timeDelta);

		void SetPlayerControlState(PlayerControl playerControl, bool state);
		void ResizeOutput(uint32 x, uint32 y);
		void Restart();
	};
}