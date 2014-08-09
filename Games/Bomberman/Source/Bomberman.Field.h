#define BLOCK_PROJ_SIZE			0.25f

#define BLOCK_FLAG_FLAME	0x02

namespace Bomberman
{
	enum Block : uint8
	{
		Empty = 0,
		Wall = 1,
		Box = 2,
		BrickWall = 3,
		BombPlace = 4,
	};

	class BlockDesc
	{
	public:
		Block block;
		uint8 state;
		uint16 flags;
		void *ptr;

		inline BlockDesc(Block _block = Block::Empty, uint16 _flags = 0, uint8 _state = 0, void* _ptr = nullptr)
			: block(_block), flags(_flags), state(_state), ptr(_ptr) {}
		inline bool IsHigh()
		{
			return this ? block == Block::Wall || block == Block::Box || block == Block::BrickWall : true;
		}
		inline bool IsGoable()
		{
			return this ? block == Block::Empty : false;
		}
		inline bool IsDestroyable()
		{
			return this ? block == Block::Box || block == Block::BrickWall : false;
		}
		inline bool IsUndestroyable()
		{
			return this ? block == Wall : true;
		}
		inline bool IsFireable()
		{
			return this ? block != Block::Wall : false;
		}

		inline void SetFlag(uint16 _flags)
		{
			if (this) flags |= _flags;
		}
		inline void ResetFlag(uint16 _flags)
		{
			if (this) flags &= ~_flags;
		}
		inline uint16 GetFlags()
		{
			return this ? flags : 0;
		}
	};

	class Field
	{
	private:
		int16x2 dim;

		BlockDesc *blocksMatrix;
		uint8 *blocksStyleMatrix;

		Render2D::RenderTarget blocksBitmap, floorBitmap, shadowsBitmap;

		uint32 uid;
		uint64 columnChangedFlags;

		bool changed;

		void RedrawBlocks();

	public:
		Field(uint16 x, uint16 y);
		inline BlockDesc* GetBlock(int16 x, int16 y)
		{
			return (x >= 0 && x < dim.x && y >= 0 && y < dim.y) ? blocksMatrix + dim.x * y + x : nullptr;
		}
		inline BlockDesc* GetBlock(int16x2 pos)
		{
			return GetBlock(pos.x, pos.y);
		}
		inline void SetBlock(int16x2 pos, Block block, void *ptr = nullptr, uint8 state = 0)
		{
			if (pos.x >= 0 && pos.x < dim.x && pos.y >= 0 && pos.y < dim.y)
			{
				blocksMatrix[dim.x * pos.y + pos.x].block = block;
				blocksMatrix[dim.x * pos.y + pos.x].ptr = ptr;
				blocksMatrix[dim.x * pos.y + pos.x].state = state;

				if (block != Block::BombPlace)
					changed = true;
			}
		}
		inline int16x2 GetSize() { return dim; }
		inline uint32 GetUID() { return uid; }

		void Update();
		void DrawFloor();
		void DrawBlocks();

		static Field *current;
	};
}