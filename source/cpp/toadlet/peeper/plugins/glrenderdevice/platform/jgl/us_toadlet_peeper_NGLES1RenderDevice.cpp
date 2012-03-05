#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/RenderTarget.h>
#include <jni.h>

using namespace toadlet::peeper;

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *env,jobject obj);
TOADLET_C_API RenderDevice *new_GLES1RenderDevice();

extern "C" {

#include "us_toadlet_peeper_NGLES1RenderDevice.h"

jfieldID NGLES1RenderDevice_nativeHandle=0;
jmethodID NGLES1RenderDevice_getRootRenderTarget=0;

void Java_us_toadlet_peeper_NGLES1RenderDevice(JNIEnv *env){
	jclass deviceClass=env->FindClass("us/toadlet/peeper/NGLES1RenderDevice");
	{
		NGLES1RenderDevice_nativeHandle=env->GetFieldID(deviceClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(deviceClass);

	jclass targetClass=env->FindClass("us/toadlet/peeper/RenderTarget");
	{
		NGLES1RenderDevice_getRootRenderTarget=env->GetMethodID(targetClass,"getRootRenderTarget","()Lus/toadlet/peeper/RenderTarget;");
	}
	env->DeleteLocalRef(targetClass);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_create(JNIEnv *env,jobject obj,jobject targetObj,jintArray options){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	jobject rootTargetObj=env->CallObjectMethod(targetObj,NGLES1RenderDevice_getRootRenderTarget);
	RenderTarget *renderTarget=new_JGLWindowRenderTarget(env,rootTargetObj);
	return device->create(renderTarget,NULL);
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_destroy(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	if(device!=NULL){
		RenderTarget *renderTarget=device->getPrimaryRenderTarget();

		device->destroy();
		delete device;

		if(renderTarget!=NULL){
			renderTarget->destroy();
			delete renderTarget;
		}

		env->SetIntField(obj,NGLES1RenderDevice_nativeHandle,(jint)NULL);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_clear(JNIEnv *env,jobject obj,int clearFlags,int clearColor){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	device->clear(clearFlags,Vector4(clearColor));
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_beginScene(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	device->beginScene();
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_endScene(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	device->endScene();
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_swap(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLES1RenderDevice_nativeHandle);

	device->swap();
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLES1RenderDevice_makeRenderDevice(JNIEnv *env,jobject obj){
	Java_us_toadlet_peeper_NGLES1RenderDevice(env);

	RenderDevice *device=new_GLES1RenderDevice();

	env->SetIntField(obj,NGLES1RenderDevice_nativeHandle,(int)device);
}

}
