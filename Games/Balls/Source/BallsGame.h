#include <extypes.h>
#include <extypes.vectors.h>
#include <extypes.queue.h>

#include <Render2D.h>

namespace BallsGame
{
	static const float hellDistance = 2.0f;

	enum class Charge : uint8
	{
		Neutral = 0,
		Positive = 1,
		Negative = 2,
	};

	struct PlayerBall
	{
		float32x2 position, speed;
		float radius;
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
		float radius;
		Charge charge;

	public:
		StaticBall() = default;
		inline StaticBall(float x, float y, float _radius, Charge _charge) : radius(_radius), charge(_charge) { position.set(x, y); }

		void CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2& translation);
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge);
		void DrawForce(Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge);
		void Draw(Render2D::Batch* batch);
	};

	class Field
	{
	private:
		static const uint32 staticBallsLimit = 64;

		Queue<StaticBall, staticBallsLimit> staticBallsQueue;

		float nextStaticBallsGroupSpawnDelta;
		Charge nextStaticBallGroupCharge;

		void spawnGameObjects();

	public:
		void Clear();
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge);
		void CollideWithPlayerBall(float timeDelta, PlayerBall& playerBall, float32x2 translation);
		void UpdateAndDraw(float posDelta, Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge);
	};

	class Background
	{
	private:
		static const uint32 blursLimit = 1024;

		struct
		{
			float32x2 position;
			float radius, posDeltaCoef;
			uint32 color;

			//inline void set(float32x2 _position, float _radius, float _posDeltaCoef)
			inline void move(float posDelta) { position.y += posDelta * posDeltaCoef; }
		} blurs[blursLimit];
		uint32 blursCount;
		float nextBlurSpawnDelta;

		void spawnBlurs();

	public:
		inline Background() : blursCount(0), nextBlurSpawnDelta(0) {}
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
		PositiveCharge = 5,
		NegativeCharge = 6,
	};

	class Game
	{
	private:
		Field field;
		Background background;
		PlayerBall playerBall;
		struct
		{
			bool left, right, up, down, positiveCharge, negativeCharge;
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