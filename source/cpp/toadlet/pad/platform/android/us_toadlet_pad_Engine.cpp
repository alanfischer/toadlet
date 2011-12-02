#include "us_toadlet_pad_Engine.h"
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

JNIEXPORT void JNICALL Java_us_toadlet_pad_Engine_destroy(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->CallIntMethod(obj,getNativeHandleEngineID);
	engine->destroy();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_Engine_installHandlers(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->CallIntMethod(obj,getNativeHandleEngineID);
	engine->installHandlers();
}
