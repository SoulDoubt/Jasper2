#ifndef _JASPER_MEMORY_H_
#define _JASPER_MEMORY_H_

#include "Common.h"
#include <new>
#include <utility>
#include <cstdint>


namespace Jasper {

using uint = unsigned int;

class Allocator {

public:

	static const size_t DefaultAlign = 4;

	Allocator() = default;
	virtual ~Allocator() = default;

	virtual void* Allocate(size_t size, size_t alignment) = 0;
	virtual void Deallocate(void* p) = 0;
	virtual size_t AllocatedSize(void* p) = 0;

	template <typename T, typename... Args>
	T* MakeNew(Args&&... args) {
		return new (Allocate(sizeof(T), alignof(T))) T{std::forward(args...)};
	}

	template<typename T>
	void MakeDelete(T* p) {
		if (p) {
			p->~T();
			Deallocate(p);
		}
	}

private:
	NON_COPYABLE(Allocator);

};


//Allocator& DefaultAllocator();

namespace Memory {

void InitMemory();
void ShutdownMemory();

inline void* AlignForward(void* p, size_t alignment) {
	uintptr_t ptr((uintptr_t)p);
	size_t modulo = ptr % alignment;
	if (modulo) {
		ptr += (uintptr_t)(alignment - modulo);
	}
	return (void*)ptr;

}

} // namespace memory

} // namespace Jasper

#endif //_JASPER_MEMORY_H_