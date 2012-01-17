#include <jni.h>

jmethodID getRootRenderTargetRenderTargetID=0;

jclass nappletClass=0;
jfieldID nappletNativeHandleID=0;

jclass nrenderDeviceClass=0;
jfieldID nrenderDeviceNativeHandleID=0;

jclass nengineClass=0;
jfieldID nengineNativeHandleID=0;

extern "C" JNIEXPORT void Java_us_toadlet_pad(JNIEnv *env){
	if(getRootRenderTargetRenderTargetID!=0){
		return;
	}

	jclass targetClass=env->FindClass("us/toadlet/peeper/RenderTarget");
	{
		getRootRenderTargetRenderTargetID=env->GetMethodID(targetClass,"getRootRenderTarget","()Lus/toadlet/peeper/RenderTarget;");
	}
	env->DeleteLocalRef(targetClass);

	nrenderDeviceClass=(jclass)env->NewGlobalRef(env->FindClass("us/toadlet/pad/NRenderDevice"));
	{
		nrenderDeviceNativeHandleID=env->GetFieldID(nrenderDeviceClass,"mNativeHandle","I");
	}

	nengineClass=(jclass)env->NewGlobalRef(env->FindClass("us/toadlet/pad/NEngine"));
	{
		nengineNativeHandleID=env->GetFieldID(nengineClass,"mNativeHandle","I");
	}

	nappletClass=(jclass)env->NewGlobalRef(env->FindClass("us/toadlet/pad/NApplet"));
	{
		nappletNativeHandleID=env->GetFieldID(nappletClass,"mNativeHandle","I");
	}
}

#include "us_toadlet_pad_AndroidApplication.cpp"
#include "us_toadlet_pad_NApplet.cpp"
#include "us_toadlet_pad_NEngine.cpp"
#include "us_toadlet_pad_NRenderDevice.cpp"

