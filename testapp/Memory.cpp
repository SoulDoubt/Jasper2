#include "Memory.h"
#include <stdlib.h>

namespace Jasper {
//
//Allocator& DefaultAllocator()
//{
//	// TODO: insert return statement here
//
//	return 
//}
namespace Memory {

void InitMemory()
{

}

void ShutdownMemory()
{

}

} // namespace Memory


struct Header {
	size_t size;
};

static inline void Fill(Header* h, void* data, size_t size) {
	h->size = size;
	size_t* p = (size_t*)(h + 1);
	while (p < data) {
		*p++;
	}
}


// Implements a heap allocator with default 4 byte alignment
class HeapAllocator : public Allocator {

public:
	HeapAllocator() = default;
	~HeapAllocator() = default;

	virtual void* Allocate(size_t size, size_t alignment = DefaultAlign) override {
		const size_t totalSize = size + alignment + sizeof(Header);
		Header* h = (Header*)malloc(totalSize);
		void* ptr = h + 1;
		ptr = Memory::AlignForward(ptr, alignment);
		return ptr;

	}

	virtual void Deallocate(void* p) override {}
	virtual size_t AllocatedSize(void* p) override {}

private:
	size_t m_totalAllocated;
};

} // Jasper