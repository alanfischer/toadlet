#include <toadlet/peeper/RenderDevice.h>

using namespace toadlet::peeper;

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj);
TOADLET_C_API RenderDevice *new_GLRenderDevice();

extern "C" {

#include "us_toadlet_pad_NRenderDevice.h"

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_NRenderDevice_create(JNIEnv *env,jobject obj,jobject targetObj,jintArray options){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,nrenderDeviceNativeHandleID);

	jobject rootTargetObj=env->CallObjectMethod(targetObj,getRootRenderTargetRenderTargetID);
	RenderTarget *renderTarget=new_JGLWindowRenderTarget(env,rootTargetObj);
	return device->create(renderTarget,NULL);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NRenderDevice_destroy(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,nrenderDeviceNativeHandleID);

	if(device!=NULL){
		RenderTarget *renderTarget=device->getPrimaryRenderTarget();

		device->destroy();
		delete device;

		if(renderTarget!=NULL){
			renderTarget->destroy();
			delete renderTarget;
		}

		env->SetIntField(obj,nrenderDeviceNativeHandleID,(jint)NULL);
	}
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_NRenderDevice_setRenderTarget(JNIEnv *env,jobject obj,jobject renderTargetObj){
	/// @todo: Implement this once we have intrusive reference counting in all non-pod objects, and can create JGL objects freely
	return false;
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NRenderDevice_clear(JNIEnv *env,jobject obj,int clearFlags,int clearColor){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,nrenderDeviceNativeHandleID);

	device->clear(clearFlags,Vector4(clearColor));
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NRenderDevice_swap(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,nrenderDeviceNativeHandleID);

	device->swap();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NRenderDevice_makeRenderDevice(JNIEnv *env,jobject obj){
	RenderDevice *device=new_GLRenderDevice();

	env->SetIntField(obj,nrenderDeviceNativeHandleID,(int)device);
}

}
