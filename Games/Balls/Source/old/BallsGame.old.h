#include <extypes.h>
#include <extypes.vectors.math.h>
#include <Render2D.h>

#define MAGNETIC_FORCE_COEF			0.04f
#define MAGNETIC_FORCE_MAX_DISTANCE 1.5f
#define MAGNETIC_FORCE_UNIT_SIZE	0.2f

#define BOUNCE_COEF_WALLS			0.8f
#define BOUNCE_COEF_CHARGED_BALLS	0.6f
#define ACCEL_PLAYERBALL_CONTROL	1.5f
#define ACCEL_GRAVITY				0.5f
#define ACCEL_FRICTION				0.25f

#define GAMEFIELD_BLOCK_SIZE	0.01f

#define BACKGROUND_BLOCK_SIZE	0.2f

#define SIDEWALL_WIDTH	0.1f
#define BACKWALL_WIDTH	0.85f
#define FIELD_WIDTH		(BACKWALL_WIDTH + 2.0f * SIDEWALL_WIDTH)
#define SIDEWALL_INNER_PERSPECCOEF	BACKWALL_WIDTH
#define SIDEWALL_OUTER_PERSPECCOEF	FIELD_WIDTH

#define GAMEFIELD_POSCOEF_OBJECT_DELETE		5.0f

#define SHADOWS_BITMAP_WIDTH	480

#define CAMERA_TARGET_OUTPUT_DISPLACEMENT_POSCOEF	0.7f
#define CAMERA_EXPON_DIST_DERCEASE_DELTA	0.04f
#define CAMERA_EXPON_DIST_DERCEASE_COEF		5.0f

inline void* operator new(size_t size, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

class GameObjectsController;

class GameObject abstract
{
	friend GameObjectsController;

public:
	enum class Message : uint32
	{
		None = 0,
		UpdateForces = 1,
		UpdateCollisions = 2,
		DrawShadows = 3,
		Draw = 4,
	};

protected:
	float32x2 position;

	virtual void Dispatch(Message message) = 0;

public:
	GameObject(float32x2 _position) : position(_position) {}
	virtual ~GameObject() {}
};

class ChargedStaticBall : public GameObject
{
private:
	float radius, charge, interactionStrength, interactionSpriteDisplacement;

	inline void UpdateForces();
	inline void UpdateCollisions();
	inline void DrawShadows();
	inline void Draw();

	virtual void Dispatch(GameObject::Message message) override;

public:
	inline ChargedStaticBall(float32x2 _position, float _radius, float _charge)
		: GameObject(_position), radius(_radius), charge(_charge), interactionStrength(0.0f), interactionSpriteDisplacement(0.0f) {}
};

class GameObjectsController
{
private:
	static const uint16 objectsLimit = 128;
	static const uint32 bufferSize = 4096;
	static const uint32 objectsAlign = 4;

	class GameObject *objectsBuffer[objectsLimit];
	void *memBuffer;
	uint16 firstObjectIdx, lastObjectIdx;
	uint32 lastObjectSize;

	inline void deleteFirstObject();
	void* allocate(uint32 _objectSize);

public:
	GameObjectsController();
	~GameObjectsController();

	void Dispatch(GameObject::Message message);
	void Clear();
	template<typename ObjectType, typename ... ObjectCreationArgTypes>
	inline void CreateObject(ObjectCreationArgTypes const ... creationArgs)
	{
		ObjectType* ptr = (ObjectType*) allocate(sizeof(ObjectType));
		new(ptr) ObjectType(creationArgs ...);
	}
};

class Textures abstract
{
public:
	static Render2D::Texture ballMask, interactionPushSprite,
		interactionPullSprite, backgroundMask;
	static bool Load();
};

struct PlayerBall
{
	float32x2 position, translation, speed, acceleration;
	float radius, charge;
	inline void UpdateTranslation(float delta)
	{
		translation = (speed + acceleration * delta / 2.0f) * delta;
		speed += acceleration * delta;
		acceleration.set(0.0f, 0.0f);
	}
	inline void CheckWallsCollisions()
	{
		if (position.x < radius)
		{
			speed.x = -speed.x * BOUNCE_COEF_WALLS;
			position.x = 2.0f * radius - position.x;
		}
		if (position.x > 1.0f - radius)
		{
			speed.x = -speed.x * BOUNCE_COEF_WALLS;
			position.x = 2.0f * (1.0f - radius) - position.x;
		}
	}
};

extern float timeDelta;
extern PlayerBall playerBall;

//--------------------------------------------------------------

