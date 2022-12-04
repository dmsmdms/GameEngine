#include <vector-math.h>
#include <string.h>

void rkMathInitIdentityMat4x4(mat4x4_t * const restrict mat) {
	memset(mat, 0, sizeof(*mat));

	for (uint_fast8_t i = 0; i < ARRAY_LEN(mat->comp); i++) {
		mat->comp[i][i] = 1.0f;
	}
}
