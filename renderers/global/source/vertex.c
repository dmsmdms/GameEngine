#include <global/api/vertex.h>
#include <string.h>

static const vertext_t squardVertices[] = {
	{{{ -0.5f, -0.5f, 0.0f }}},
	{{{ 0.5f, -0.5f, 0.0f }}},
	{{{ 0.5f, 0.5f, 0.0f }}},
	{{{ -0.5f, 0.5f, 0.0f }}},
};

static const uint16_t squardIndeces[] = {
	0, 1, 2, 2, 3, 0
};

uint_fast32_t indexCount = ARRAY_LEN(squardIndeces);
uint_fast32_t vertexCount = ARRAY_LEN(squardVertices);
uint_fast32_t instanceCount = 1;
uint_fast32_t firstIndex = 0;
uint_fast32_t firstVertex = 0;
uint_fast32_t firstInstance = 0;

void rkFillVertexBuffer(vertext_t * const restrict vertices) {
	memcpy(vertices, squardVertices, sizeof(squardVertices));
}

void rkFillIndexBuffer(uint16_t * const restrict indeces) {
	memcpy(indeces, squardIndeces, sizeof(squardIndeces));
}
