#include <toadlet/peeper/RenderDevice.h>

using namespace toadlet::peeper;

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj);

extern "C" {

#include "us_toadlet_pad_RenderDevice.h"

RenderTarget::ptr renderTarget;

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_RenderDevice_create(JNIEnv *env,jobject obj,jobject targetObj,jintArray options){
	RenderDevice *device=(RenderDevice*)env->CallIntMethod(obj,getNativeHandleRenderDeviceID);
	jobject rootTargetObj=env->CallObjectMethod(targetObj,getRootRenderTargetRenderTargetID);
	renderTarget=RenderTarget::ptr(new_JGLWindowRenderTarget(env,rootTargetObj));
	return device->create(renderTarget,NULL);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_RenderDevice_destroy(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->CallIntMethod(obj,getNativeHandleRenderDeviceID);
	device->destroy();
	renderTarget=NULL;
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_RenderDevice_setRenderTarget(JNIEnv *env,jobject obj,jobject renderTargetObj){
	/// @todo: Implement this once we have intrusive reference counting in all non-pod objects, and can create JGL objects freely
	return false;
}

}
