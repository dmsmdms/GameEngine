#include <global/api/renderer.h>
#include <autoconf.h>
#include <stdbool.h>
#include <defines.h>
#include <stdlib.h>
#include <window.h>
#include <jni.h>

static JNIEnv * restrict pEnvGlobal = NULL;
static jclass classGlobal = NULL;

static void rkConstructor(void) {
	rkWindowCreate();
	rkRendererCreate();
}

static void rkDestructor(void) {
	rkRendererDestroy();
	rkWindowDestroy();
}

JNIEXPORT void JNICALL createModule(JNIEnv * const restrict pEnv, const jobject obj) {
	const JNIEnv env = *pEnv;
	classGlobal = env->GetObjectClass(pEnv, obj);
	pEnvGlobal = pEnv;
	rkConstructor();
}

JNIEXPORT void JNICALL destroyModule(UNUSED JNIEnv * const restrict pEnv, UNUSED const jobject obj) {
	rkDestructor();
}

_Noreturn void exit(const int code) {
	const JNIEnv env = *pEnvGlobal;
	const jmethodID funcId = env->GetMethodID(pEnvGlobal, classGlobal, "finishAffinity", "(Ljava/lang/Void;)V");
	env->CallStaticVoidMethod(pEnvGlobal, classGlobal, funcId, code);
	while (true);
}
