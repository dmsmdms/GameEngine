#include <simple-math.h>
#include <stdint.h>

typedef union {
	float f;
	uint32_t i;
} fi_union_t;

float rkFabs(const float x) {
	fi_union_t u = { x };
	u.i &= -1U / 2;
	return u.f;
}
