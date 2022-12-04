#include <vector-math.h>
#include <simple-math.h>
#include <asserts.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tests.h>
#include <math.h>

static mat4x4_t identityMat4x4 = {
	.comp = {
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	},
};

static void rkTestMathMulMat(float * const restrict out, const float * const restrict in1,
							 const float * const restrict in2, const uint_fast8_t size)
{
	for (uint_fast8_t i = 0; i < size; i++) {
		for(uint_fast8_t j = 0; j < size; j++) {
			float x = 0.0f;

			for(uint_fast8_t k = 0; k < size; k++) {
				x += in1[i * size + k] * in2[k * size + j];
			}

			out[i * size + j] = x;
		}
	}
}

static uint_fast8_t rkTestMathCmpMat(const float * const restrict in1,
									 const float * const restrict in2,
									 const uint_fast8_t size)
{
	for (uint_fast8_t i = 0; i < size * size; i++) {
		if (fabs(in1[i] - in2[i]) > RK_FLOAT_EPS) {
			return i;
		}

		printf("%.2f\n", fabs(in1[i] - in2[i]));
	}

	return RK_RESULT_SUCCESS;
}

static void rkTestPrintMat(const float * const restrict mat, const uint_fast8_t size) {
	for (uint_fast8_t i = 0; i < size; i++) {
		for (uint_fast8_t j = 0; j < size; j++) {
			printf("%.2f ", mat[i * size + j]);
		}

		putchar('\n');
	}
}

static void rkTestGenRandomMat(float * const restrict mat, const uint_fast8_t size) {
	for (uint_fast8_t i = 0; i < size; i++) {
		for (uint_fast8_t j = 0; j < size; j++) {
			mat[i * size + j] = rand() % 10;
		}
	}
}

void rkTestMathInitIdentityMat4x4(void) {
	mat4x4_t mat = {};
	rkMathInitIdentityMat4x4(&mat);

	const int result = memcmp(&mat, &identityMat4x4, sizeof(mat));
	RK_ASSERT(result, NULL);
}

void rkTestMathMulMat4x4(void) {
	mat4x4_t in1 = {};
	rkTestGenRandomMat(*in1.comp, ARRAY_LEN(in1.vec));

	mat4x4_t in2 = {};
	rkTestGenRandomMat(*in2.comp, ARRAY_LEN(in2.vec));

	mat4x4_t recv = {};
	rkMathMulMat4x4(&recv, &in1, &in2);

	mat4x4_t expect = {};
	rkTestMathMulMat(*expect.comp, *in1.comp, *in2.comp, ARRAY_LEN(expect.vec));

	const uint_fast8_t result = rkTestMathCmpMat(*recv.comp, *expect.comp,
												 ARRAY_LEN(expect.vec));

	//if (result != RK_RESULT_SUCCESS) {
		puts("Received matrix:");
		rkTestPrintMat(*recv.comp, ARRAY_LEN(recv.vec));

		puts("Expected matrix:");
		rkTestPrintMat(*expect.comp, ARRAY_LEN(expect.vec));
	//}

	RK_ASSERT(result, NULL);
}
