#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/RenderTarget.h>
#include <jni.h>

using namespace toadlet::peeper;

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *env,jobject obj);
TOADLET_C_API RenderDevice *new_GLRenderDevice();

extern "C" {

#include "us_toadlet_peeper_NGLRenderDevice.h"

jfieldID NGLRenderDevice_nativeHandle=0;
jmethodID NGLRenderDevice_getRootRenderTarget=0;

void Java_us_toadlet_peeper_NGLRenderDevice(JNIEnv *env){
	jclass deviceClass=env->FindClass("us/toadlet/peeper/NGLRenderDevice");
	{
		NGLRenderDevice_nativeHandle=env->GetFieldID(deviceClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(deviceClass);

	jclass targetClass=env->FindClass("us/toadlet/peeper/RenderTarget");
	{
		NGLRenderDevice_getRootRenderTarget=env->GetMethodID(targetClass,"getRootRenderTarget","()Lus/toadlet/peeper/RenderTarget;");
	}
	env->DeleteLocalRef(targetClass);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_peeper_NGLRenderDevice_create(JNIEnv *env,jobject obj,jobject targetObj,jintArray options){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLRenderDevice_nativeHandle);

	jobject rootTargetObj=env->CallObjectMethod(targetObj,NGLRenderDevice_getRootRenderTarget);
	RenderTarget *renderTarget=new_JGLWindowRenderTarget(env,rootTargetObj);
	return device->create(renderTarget,NULL);
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLRenderDevice_destroy(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLRenderDevice_nativeHandle);

	if(device!=NULL){
		RenderTarget *renderTarget=device->getPrimaryRenderTarget();

		device->destroy();
		delete device;

		if(renderTarget!=NULL){
			renderTarget->destroy();
			delete renderTarget;
		}

		env->SetIntField(obj,NGLRenderDevice_nativeHandle,(jint)NULL);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLRenderDevice_clear(JNIEnv *env,jobject obj,int clearFlags,int clearColor){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLRenderDevice_nativeHandle);

	device->clear(clearFlags,Vector4(clearColor));
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLRenderDevice_swap(JNIEnv *env,jobject obj){
	RenderDevice *device=(RenderDevice*)env->GetIntField(obj,NGLRenderDevice_nativeHandle);

	device->swap();
}

JNIEXPORT void JNICALL Java_us_toadlet_peeper_NGLRenderDevice_makeRenderDevice(JNIEnv *env,jobject obj){
	RenderDevice *device=new_GLRenderDevice();

	env->SetIntField(obj,NGLRenderDevice_nativeHandle,(int)device);
}

}
