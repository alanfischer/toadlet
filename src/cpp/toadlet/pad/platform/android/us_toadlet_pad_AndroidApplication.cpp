#include "us_toadlet_pad_AndroidApplication.h"
#include <toadlet/pad/Application.h>
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

#include <android/log.h>

using namespace toadlet::pad;
using namespace toadlet::tadpole::handler;

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeEngine(JNIEnv *env,jobject obj){
	Engine *engine=new Engine();
	jobject jengine=NULL;
	jobject jassetManager=NULL;
	
	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		jmethodID initID=env->GetMethodID(engineClass,"<init>","(I)V");
		jengine=env->NewObject(engineClass,initID,(int)engine);
	}
	env->DeleteLocalRef(engineClass);

	jclass contextClass=env->FindClass("android/content/Context");
	{
		jmethodID getAssetsID=env->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
		jassetManager=env->CallObjectMethod(obj,getAssetsID);
	}
	env->DeleteLocalRef(contextClass);

	/// @todo: This should be moved away from here, perhaps to a JAndroidApplication post-engine init method
	AndroidAssetArchive::ptr assetArchive=AndroidAssetArchive::ptr(new AndroidAssetArchive(env,jassetManager));
	engine->getArchiveManager()->manage(shared_static_cast<Archive>(assetArchive));
	AndroidTextureHandler::ptr textureHandler=AndroidTextureHandler::ptr(new AndroidTextureHandler(engine->getTextureManager(),env));
	engine->getTextureManager()->setDefaultStreamer(textureHandler);

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
