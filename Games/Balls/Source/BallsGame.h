#include <extypes.h>
#include <extypes.vectors.h>
#include <StaticCyclicQueue.h>
#include <StaticList.h>

#include <Render2D.h>

namespace BallsGame
{
	constexpr float32 hellDistance = -2.0f;

	enum class Charge : uint8
	{
		Neutral = 0,
		Positive = 1,
		Negative = 2,
	};

	struct PlayerBall
	{
		float32x2 position, speed;
		float32 radius;
	};

	class GameObject abstract
	{
	protected:
		float32x2 position;

	public:
		inline float32x2 GetPosition() { return position; }
		inline void Move(float32 posDelta) { position.y += posDelta; }
	};

	class StaticBall : public GameObject
	{
	private:
		float32 radius;
		Charge charge;

	public:
		StaticBall() = default;
		inline StaticBall(float32 x, float32 y, float32 _radius, Charge _charge) : radius(_radius), charge(_charge) { position.set(x, y); }

		float32 GetPlayerBallCollisionDistance(PlayerBall& playerBall, const float32x2& translation, float32 translationLength);
		void CollideWithPlayerBall(float32 collisionDistance, PlayerBall& playerBall, float32x2& translation, float32 translationLength);
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float32 playerBallCharge);
		void DrawForce(Render2D::Batch* batch, const PlayerBall& playerBall, float32 playerBallCharge);
		void Draw(Render2D::Batch* batch);
	};

	class Field
	{
	private:
		static constexpr uint32 staticBallsLimit = 64;
		StaticCyclicQueue<StaticBall, staticBallsLimit> staticBallsQueue;

		float32 nextStaticBallsGroupSpawnDelta;
		Charge nextStaticBallGroupCharge;

		void spawnGameObjects();

	public:
		void Initialize();
		float32x2 GetForceAppliedToPlayerBall(const PlayerBall& playerBall, float32 playerBallCharge);
		void CollideWithPlayerBall(PlayerBall& playerBall, float32x2 translation);
		void UpdateAndDraw(float32 posDelta, Render2D::Batch* batch, const PlayerBall& playerBall, float32 playerBallCharge);
	};

	class Background
	{
	private:
		static constexpr uint32 blursLimit = 64;
		struct BlurDesc
		{
			float32x2 position;
			float32 depth, radius;
			BlurDesc() = default;
			inline BlurDesc(float32x2 _position, float32 _depth, float32 _radius) : position(_position), depth(_depth), radius(_radius) {}
		};
		friend bool operator > (const BlurDesc& a, const BlurDesc& b);
		StaticOrderedList<BlurDesc, blursLimit> blursList;
		float32 nextBlurSpawnDelta;
		void spawnBlurs();

	public:
		inline Background() : nextBlurSpawnDelta(0.0f) {}
		void Initialize();
		void UpdateAndDraw(float32 posDelta, float32 cameraDelta, Render2D::Batch* batch);
	};
	inline bool operator > (const Background::BlurDesc& a, const Background::BlurDesc& b)
	{
		return a.depth > b.depth;
	}

	class Hell
	{
	private:
		static constexpr uint32 blursLimit = 64;
		struct BlurDesc
		{
			float32x2 position;
			float32 radius, lifeTime;
			BlurDesc() = default;
			inline BlurDesc(float32x2 _position, float32 _radius, float32 _lifeTime) : position(_position), radius(_radius), lifeTime(_lifeTime) {}
		};
		StaticCyclicQueue<BlurDesc, blursLimit> blursQueue;
		float32 nextBlurSpawnDelta;
		void spawnBlurs();

		float32 distance;

	public:
		void Initialize();
		void UpdateAndDraw(float32 posDelta, float32 timeDelta, Render2D::Batch* batch);
	};

	enum class PlayerControl
	{
		None, Left, Right, Up, Down,
		PositiveCharge, NegativeCharge, Jump,
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
			bool left, right, up, down, positiveCharge, negativeCharge, jump;
		} controls;
		float32 aspect, cameraDelta;
		float32 globalPosition, maxGlobalPosition;

		static Render2D::Device device;
		Render2D::SwapChain swapChain;
		uint32x2 outputSize;

	public:
		bool Create(void* outputHWnd, uint32 outputSizeX, uint32 outputSizeY);
		void ResizeOutput(uint32 x, uint32 y);
		void Update(float32 timeDelta);
		void SetPlayerControlState(PlayerControl playerControl, bool state);
		void Restart();
	};
}

extern uint32 frameid;