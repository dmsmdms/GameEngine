#include <vector-math.h>
#include <xmmintrin.h>

void rkMathMulMat4x4(mat4x4_t * const restrict out, const mat4x4_t * const restrict in1,
					 const mat4x4_t * const restrict in2)
{
	const __m128 in1vec[] = {
		_mm_load_ps(in1->vec[0].comp),
		_mm_load_ps(in1->vec[1].comp),
		_mm_load_ps(in1->vec[2].comp),
		_mm_load_ps(in1->vec[3].comp),
	};

	for (uint_fast8_t i = 0; i < ARRAY_LEN(out->vec); i++) {
		const __m128 in2vec = _mm_load_ps(in2->vec[i].comp);
		const __m128 e0 = _mm_shuffle_ps(in2vec, in2vec, _MM_SHUFFLE(0, 0, 0, 0));
		const __m128 e1 = _mm_shuffle_ps(in2vec, in2vec, _MM_SHUFFLE(1, 1, 1, 1));
		const __m128 e2 = _mm_shuffle_ps(in2vec, in2vec, _MM_SHUFFLE(2, 2, 2, 2));
		const __m128 e3 = _mm_shuffle_ps(in2vec, in2vec, _MM_SHUFFLE(3, 3, 3, 3));

		const __m128 m0 = _mm_mul_ps(in1vec[0], e0);
		const __m128 m1 = _mm_mul_ps(in1vec[1], e1);
		const __m128 m2 = _mm_mul_ps(in1vec[2], e2);
		const __m128 m3 = _mm_mul_ps(in1vec[3], e3);

		const __m128 a0 = _mm_add_ps(m0, m1);
		const __m128 a1 = _mm_add_ps(m2, m3);
		const __m128 a2 = _mm_add_ps(a0, a1);
		_mm_store_ps(out->vec[i].comp, a2);
	}
}
