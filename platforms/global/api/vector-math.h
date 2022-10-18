#pragma once

#include <defines.h>

typedef union PACKED {
	float component[3];
	struct PACKED {
		float x;
		float y;
		float z;
	};
	struct PACKED {
		float r;
		float g;
		float a;
	};
} vec3_t;
