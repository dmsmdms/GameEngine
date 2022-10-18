#pragma once

#include <vector-math.h>
#include <stdint.h>

typedef struct PACKED {
	vec3_t inPosition;
} vertext_t;

extern uint_fast32_t indexCount;
extern uint_fast32_t vertexCount;
extern uint_fast32_t instanceCount;
extern uint_fast32_t firstIndex;
extern uint_fast32_t firstVertex;
extern uint_fast32_t firstInstance;

void rkFillVertexBuffer(vertext_t * const restrict vertices);
void rkFillIndexBuffer(uint16_t * const restrict indeces);
