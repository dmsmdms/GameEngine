#include <global/api/renderer.h>
#include <stdbool.h>
#include <options.h>
#include <asserts.h>
#include <logger.h>
#include <window.h>
#include <stdlib.h>

static void rkConstructor(void) {
	rkWindowCreate();
	rkRendererCreate();
}

static void rkDestructor(void) {
	rkRendererDestroy();
	rkWindowDestroy();
}

int main(const int argc, char ** const argv) {
	rkParseCmdOptions(argc, argv);

	const int result = atexit(rkDestructor);
	RK_ASSERT(result != RK_RESULT_SUCCESS, NULL);
	rkConstructor();

	while (appIsRunning) {
		rkRendererDraw();
		rkWindowGetEvents();
	}

	return RK_RESULT_SUCCESS;
}
