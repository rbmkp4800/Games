#define MONSTER_PATHBUFF_SIZE	32

namespace Bomberman
{
	class ICharacter : public IGameObject
	{
	protected:
		float32x2 positionF;
		int16x2 position;
		float displacement;
		Render2D::Direction direction;
		uint8 speed;

		virtual Render2D::Direction GetNextDirection() = 0;

	public:
		inline ICharacter(uint16 x, uint16 y, uint8 _speed)
			: position(x, y), positionF((float) x + 0.5f, (float) y + 1), direction(Render2D::Direction::None), displacement(0.0f), speed(_speed) {}
		virtual GOUpdateResult Update(uint16 delta) override;

		inline int16x2 GetPosition() { return position; }
	};

	class IMonster : public ICharacter
	{
	private:
		uint32 lastPathUpdateFieldUid;
		Render2D::Direction path[MONSTER_PATHBUFF_SIZE];
		uint16x2 targetPos;
		uint8 pathIdx;
		bool unreachable;

		void UpdatePath();

	protected:
		virtual Render2D::Direction GetNextDirection() override;

	public:
		inline IMonster(uint16 x, uint16 y, uint8 speed) : ICharacter(x, y, speed), lastPathUpdateFieldUid(UINT32_MAX) {}
		virtual GOUpdateResult Update(uint16 delta) override;

		static void Init();
	};

	class Ghost : public IMonster
	{
	private:
		uint16 animTick;
		uint8 currentAnimFrame;

	public:
		inline Ghost(uint16 x, uint16 y) : IMonster(x, y, 20), currentAnimFrame(0), animTick(0) {}
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class Jock : public IMonster
	{
	private:
		uint16 animTick;
		uint8 currentAnimFrame;
		int8 animStep;

	public:
		inline Jock(uint16 x, uint16 y) : IMonster(x, y, 10), currentAnimFrame(0), animTick(0), animStep(1) {}
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class Player : public ICharacter
	{
	private:
		uint16 animTick;
		uint8 currentAnimFrame;

		virtual Render2D::Direction GetNextDirection() override;

	public:
		Player(uint16 x, uint16 y);
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};
}