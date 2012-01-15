#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

TOADLET_C_API AudioDevice *new_JAudioDevice(JNIEnv *env,jobject obj);

extern "C" {

#include "us_toadlet_pad_NEngine.h"

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_destroy(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	if(engine!=NULL){
		engine->destroy();
		delete engine;

		env->SetIntField(obj,nengineNativeHandleID,(jint)engine);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_installHandlers(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	engine->installHandlers();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	RenderDevice *device=NULL;
	if(deviceObj!=NULL){
		if(env->IsInstanceOf(deviceObj,nrenderDeviceClass)){
			device=(RenderDevice*)env->GetIntField(deviceObj,nrenderDeviceNativeHandleID);
		}
//		else{
//			device=new JRenderDevice(env,deviceObj);
//		}
	}
	
	engine->setRenderDevice(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setAudioDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	AudioDevice *device=NULL;
	if(deviceObj!=NULL){
//		if(env->IsInstanceOf(deviceObj,nAudioDeviceClass)){
//			device=(AudioDevice*)env->GetIntField(deviceObj,nAudioDeviceNativeHandleID);
//		}
//		else{
			device=new_JAudioDevice(env,deviceObj);
//		}
	}
	
	engine->setAudioDevice(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_makeEngine(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);

	Engine *engine=new Engine(true,false);
	
	env->SetIntField(obj,nengineNativeHandleID,(jint)engine);
}

}
