#pragma once

#include <defines.h>
#include <stdint.h>

typedef union PACKED {
	float comp[3];
	struct PACKED {
		float x;
		float y;
		float z;
	};
	struct PACKED {
		float r;
		float g;
		float b;
	};
} vec3_t;

typedef union {
	float comp[4];
	struct PACKED {
		float x;
		float y;
		float z;
		float w;
	};
	struct PACKED {
		float r;
		float g;
		float b;
		float a;
	};
} vec4_t;

typedef union {
	float comp[4][4];
	vec4_t vec[4];
} mat4x4_t;

void rkMathInitIdentityMat4x4(mat4x4_t * const restrict mat);
void rkMathMulMat4x4(mat4x4_t * const restrict out, const mat4x4_t * const restrict in1,
					 const mat4x4_t * const restrict in2);
