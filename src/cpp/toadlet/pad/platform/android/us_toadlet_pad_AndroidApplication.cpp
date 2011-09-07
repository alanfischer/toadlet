#include "us_toadlet_pad_AndroidApplication.h"
#include <toadlet/pad/Application.h>

#include <android/log.h>

using namespace toadlet::pad;

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeEngine(JNIEnv *env,jobject obj){
	Engine *engine=new Engine();
	jobject jengine=NULL;
	
	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		jmethodID initID=env->GetMethodID(engineClass,"<init>","(I)V");
		jengine=env->NewObject(engineClass,initID,(int)engine);
	}
	env->DeleteLocalRef(engineClass);
	
	return jengine;
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_deleteEngine(JNIEnv *env,jobject obj,jobject engineObj){
	Engine *engine=NULL;

	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		jmethodID getNativeHandleID=env->GetMethodID(engineClass,"getNativeHandle","()I");
		engine=(Engine*)env->CallIntMethod(engineObj,getNativeHandleID);
	}
	env->DeleteLocalRef(engineClass);
	
	delete engine;
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeRenderDevice(JNIEnv *env,jobject obj){
	RenderDevice *device=new_GLRenderDevice();
	jobject jdevice=NULL;

	jclass deviceClass=env->FindClass("us/toadlet/pad/RenderDevice");
	{
		jmethodID initID=env->GetMethodID(deviceClass,"<init>","(I)V");
		jdevice=env->NewObject(deviceClass,initID,(int)device);
	}
	env->DeleteLocalRef(deviceClass);

	return jdevice;
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_deleteRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	RenderDevice *device=NULL;

	jclass deviceClass=env->FindClass("us/toadlet/pad/RenderDevice");
	{
		jmethodID getNativeHandleID=env->GetMethodID(deviceClass,"getNativeHandle","()I");
		device=(RenderDevice*)env->CallIntMethod(deviceObj,getNativeHandleID);
	}
	env->DeleteLocalRef(deviceClass);
	
	delete device;
}
