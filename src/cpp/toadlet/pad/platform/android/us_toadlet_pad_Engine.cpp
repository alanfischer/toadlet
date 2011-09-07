#include "us_toadlet_pad_Engine.h"
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

JNIEXPORT void JNICALL Java_us_toadlet_pad_Engine_installHandlers(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);
	Engine *engine=(Engine*)env->CallIntMethod(obj,getNativeHandleEngineID);
	engine->installHandlers();
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_Engine_setRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Java_us_toadlet_pad(env);
	Engine *engine=(Engine*)env->CallIntMethod(obj,getNativeHandleEngineID);
	RenderDevice *device=(RenderDevice*)env->CallIntMethod(deviceObj,getNativeHandleRenderDeviceID);
	return engine->setRenderDevice(device);
}
