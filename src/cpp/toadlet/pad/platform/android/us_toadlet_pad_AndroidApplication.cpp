#include "us_toadlet_pad_AndroidApplication.h"
#include <toadlet/pad/Application.h>

#include <android/log.h>

using namespace toadlet::pad;

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeEngine(JNIEnv *env,jobject obj){
	Engine *engine=new Engine();
	jobject jengine=NULL;
	
	jclass nengineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		jmethodID initID=env->GetMethodID(nengineClass,"<init>","(I)V");
		jengine=env->NewObject(nengineClass,initID,(int)engine);
	}
	env->DeleteLocalRef(nengineClass);
	
	return jengine;
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeRenderDevice(JNIEnv *env,jobject obj){
	RenderDevice *device=new_GLRenderDevice();
	jobject jdevice=NULL;
	
	jclass ndeviceClass=env->FindClass("us/toadlet/pad/RenderDevice");
	{
		jmethodID initID=env->GetMethodID(ndeviceClass,"<init>","(I)V");
		jdevice=env->NewObject(ndeviceClass,initID,(int)device);
	}
	env->DeleteLocalRef(ndeviceClass);

	return jdevice;
}
