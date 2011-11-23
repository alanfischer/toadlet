#include <jni.h>

jmethodID getRootRenderTargetRenderTargetID=0;
jmethodID getNativeHandleRenderDeviceID=0;
jmethodID getNativeHandleAudioDeviceID=0;
jmethodID getNativeHandleEngineID=0;
jmethodID getNativeHandleApplicationID=0;
jfieldID nativeHandleAppletID=0;

extern "C" JNIEXPORT void Java_us_toadlet_pad(JNIEnv *env){
	jclass targetClass=env->FindClass("us/toadlet/peeper/RenderTarget");
	{
		getRootRenderTargetRenderTargetID=env->GetMethodID(targetClass,"getRootRenderTarget","()Lus/toadlet/peeper/RenderTarget;");
	}
	env->DeleteLocalRef(targetClass);

	jclass deviceClass=env->FindClass("us/toadlet/pad/RenderDevice");
	{
		getNativeHandleRenderDeviceID=env->GetMethodID(deviceClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(deviceClass);

	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		getNativeHandleEngineID=env->GetMethodID(engineClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(engineClass);

	jclass appClass=env->FindClass("us/toadlet/pad/AndroidApplication");
	{
		getNativeHandleApplicationID=env->GetMethodID(appClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(appClass);

	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		nativeHandleAppletID=env->GetFieldID(appletClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(appletClass);
}

#include "us_toadlet_pad_AndroidApplication.cpp"
#include "us_toadlet_pad_NApplet.cpp"
#include "us_toadlet_pad_RenderDevice.cpp"
#include "us_toadlet_pad_Engine.cpp"

