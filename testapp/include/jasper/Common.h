#ifndef _JASPER_COMMON_H_
#define _JASPER_COMMON_H_

#include <cstdio>

// Some Macros

#define ALIGN16 __declspec(align(16))

#define ALIGN_16_OPERATORS void* operator new(size_t sz){ \
        return _aligned_malloc(sz, 16); \
    } \
    void operator delete(void* p) { \
        _aligned_free(p); \
    } \
     
#define NON_COPYABLE(X)   X(const X&) = delete; \
    X& operator=(const X&) = delete;            \
    X(X&&) = default;                           \
    X& operator=(X&&) = default;
    

using uint = unsigned int;

#define DEBUG_DRAW_PHYSICS



//#define FORCE_INLINE __forceinline

#endif
