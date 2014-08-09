namespace Bomberman
{
	class SmartAllocator abstract
	{
	public:
		static void Init();
		static void* Allocate(uint16 size);
		static void Free(void* ptr);
	};

	class SmartAllocatorObject abstract
	{
	public:
		inline void* operator new (size_t size)
		{
			return SmartAllocator::Allocate((uint16)size);
		}
		inline void operator delete (void* ptr)
		{
			SmartAllocator::Free(ptr);
		}
	};
}