#define MAX_GOCOUNT_PER_CONTROLLER	32
#define GetGODrawPriority(yyy)	((uint16)((float)(yyy) * 10.0f + 16384.0f))

namespace Bomberman
{
	enum class GOUpdateType : uint8
	{
		NoRedraw = 0,
		Redraw = 1,
		PositionChanged = 2,
		Release = 3,
	};

	struct GOUpdateResult
	{
		GOUpdateType type;
		uint8 reserved;
		uint16 drawPriority;
		inline GOUpdateResult(GOUpdateType _type, uint16 _drawPriority = 0)
			: type(_type), drawPriority(_drawPriority) {}
	};

	class IGameObject abstract : public SmartAllocatorObject
	{
	public:
		virtual GOUpdateResult Update(uint16 delta) = 0;
		virtual void Draw() = 0;
	};

	class GameObjectsController
	{
	private:
		IGameObject *goBase[MAX_GOCOUNT_PER_CONTROLLER];
		uint16 goDrawPriorityBase[MAX_GOCOUNT_PER_CONTROLLER];
		int16 goCount;
		bool objectsAdded;

	public:
		inline GameObjectsController() : goCount(0), objectsAdded(false) {}
		bool Update(uint16 delta);
		void Draw();
		void Add(IGameObject* object);
	};

	class GameObjectsControllerNoDepth
	{
	private:
		IGameObject *goBase[MAX_GOCOUNT_PER_CONTROLLER];
		int16 goCount;
		bool objectsAdded;

	public:
		inline GameObjectsControllerNoDepth() : goCount(0), objectsAdded(false) {}
		bool Update(uint16 delta);
		void Draw();
		void Add(IGameObject* object);
	};

	class AnimatedSprite : public IGameObject
	{
	private:
		rectf32 destRect;
		Render2D::Texture *bitmap;
		uint16 period, tick;
		uint8 frameCount, currentFrame;

	public:
		inline AnimatedSprite(Render2D::Texture* _bitmap, rectf32* rect, uint8 _frameCount, uint16 _period)
			: bitmap(_bitmap), destRect(*rect), frameCount(_frameCount), currentFrame(0), period(_period), tick(0) {}
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class ExpandingFadingSprite : public IGameObject
	{
	private:
		rectf32 destRect;
		Render2D::Texture *bitmap;
		uint16 timer, tick;
		float expansion;
		float opacity;

	public:
		inline ExpandingFadingSprite(Render2D::Texture* _bitmap, rectf32* _destRect, float _expansion, uint16 _timer, float _opacity = 1.0f)
			: bitmap(_bitmap), destRect(*_destRect), expansion(_expansion), timer(_timer), tick(0), opacity(_opacity) {}
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class AnimatedExpandingFadingSprite : public IGameObject
	{
	private:
		rectf32 destRect;
		Render2D::Texture *bitmap;
		uint16 timer, tick;
		float expansion;
		float opacity;
		uint16 animPeriod, animTick;
		uint8 frameCount, currentFrame;
		enum : uint8 {Cycled, Once, Stopped} animMode;

	public:
		inline AnimatedExpandingFadingSprite(Render2D::Texture* _bitmap, rectf32* _destRect, float _expansion,
			uint16 _timer, uint8 _frameCount, uint16 _period, float _opacity = 1.0f)
			: bitmap(_bitmap), destRect(*_destRect), expansion(_expansion), timer(_timer), tick(0),
			frameCount(_frameCount), currentFrame(0), animPeriod(_period), opacity(_opacity), animMode(Once), animTick(0) {}
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class FadingSprite : public IGameObject
	{
	private:
		rectf32 destRect;
		Render2D::Texture *bitmap;
		uint32 timer, tick;
		float opacity;
		uint8 lastOpacity;

	public:
		FadingSprite(Render2D::Texture* _bitmap, rectf32* _destRect, uint32 _timer, float _opacity = 1.0f);
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};

	class Bomb : public IGameObject
	{
	private:
		int16x2 position;
		uint16 tick, timer;
		uint8 power;

	public:
		Bomb(uint16 x, uint16 y, uint16 _timer, uint8 _power);
		virtual GOUpdateResult Update(uint16 delta) override;
		virtual void Draw() override;
	};
}