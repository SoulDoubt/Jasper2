// SIMD Math Enabled Classes

#include <intrin.h>

namespace SIMD {

typedef __m128 simd_float4;

struct Vector3 {

	union {
		float x, y, z;
		simd_float4 m_vec;
	};

};


}
