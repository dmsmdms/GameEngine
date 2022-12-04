#include <asserts.h>
#include <defines.h>
#include <options.h>
#include <logger.h>
#include <tests.h>

static const RkTest tests[] = {
	RK_TEST(rkTestMathInitIdentityMat4x4),
	RK_TEST(rkTestMathMulMat4x4),
};

int main(const int argc, char ** const argv) {
	rkParseCmdOptions(argc, argv);

	for (uint_fast8_t i = 0; i < ARRAY_LEN(tests); i++) {
		const RkTest * const restrict test = &tests[i];
		rkLog(RK_LOG_LEVEL_DEBUG, "Run test %s", test->name);
		test->func();
	}

	rkLog(RK_LOG_LEVEL_INFO, "All tests done!");
	return RK_RESULT_SUCCESS;
}
