#pragma once

#define RK_TEST(func) { func, #func }

typedef void (* RkTestCallback)(void);

typedef struct {
	RkTestCallback func;
	const char * name;
} RkTest;

void rkTestMathInitIdentityMat4x4(void);
void rkTestMathMulMat4x4(void);
