#include <extypes.h>
#include <extypes.vectors.h>
#include <StaticCyclicQueue.h>
#include <StaticList.h>

#include <Render2D.h>

namespace BallsGame
{
	static const float hellDistance = -2.0f;

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

		float GetPlayerBallCollisionDistance(PlayerBall& playerBall, const float32x2& translation, float translationLength);
		void CollideWithPlayerBall(float collisionDistance, PlayerBall& playerBall, float32x2& translation, float translationLength);
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge);
		void DrawForce(Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge);
		void Draw(Render2D::Batch* batch);
	};

	class Field
	{
	private:
		static const uint32 staticBallsLimit = 64;
		StaticCyclicQueue<StaticBall, staticBallsLimit> staticBallsQueue;

		float nextStaticBallsGroupSpawnDelta;
		Charge nextStaticBallGroupCharge;

		void spawnGameObjects();

	public:
		void Initialize();
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float playerBallCharge);
		void CollideWithPlayerBall(PlayerBall& playerBall, float32x2 translation);
		void UpdateAndDraw(float posDelta, Render2D::Batch* batch, const PlayerBall& playerBall, float playerBallCharge);
	};

	class Background
	{
	private:
		static const uint32 blursLimit = 64;
		struct BlurDesc
		{
			float32x2 position;
			float depth, radius;
		};
		friend bool operator > (const BlurDesc& a, const BlurDesc& b);
		StaticOrderedList<BlurDesc, blursLimit> blursList;
		float nextBlurSpawnDelta;
		void spawnBlurs();

	public:
		inline Background() : nextBlurSpawnDelta(0.0f) {}
		void Initialize();
		void UpdateAndDraw(float posDelta, float cameraDelta, Render2D::Batch* batch);
	};
	inline bool operator > (const Background::BlurDesc& a, const Background::BlurDesc& b)
	{
		return a.depth > b.depth;
	}

	class Hell
	{
	private:
		static const uint32 blursLimit = 64;
		struct BlurDesc
		{
			float32x2 position;
			float radius, lifeTime;
		};
		StaticCyclicQueue<BlurDesc, blursLimit> blursQueue;
		float nextBlurSpawnDelta;
		void spawnBlurs();

		float distance;

	public:
		void Initialize();
		void UpdateAndDraw(float posDelta, float timeDelta, Render2D::Batch* batch);
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
		Hell hell;
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

extern uint32 frameid;