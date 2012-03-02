#include "../../src/Input.h"
#include <jni.h>

#if ANDROID_NDK_API_LEVEL>=9
extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *activity,void *savedState,size_t savedStateSize){
	Logger::alert("ANativeActivity_onCreate");

	AndroidApplication *app=new AndroidApplication();
	app->setApplet(new Input(app,""));
	app->setNativeActivity(activity);
}
#else
extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_input_Input_createApplet(JNIEnv *env,jobject obj){
	Logger::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Logger::alert("creating applet");
	Applet *applet=new Input(app);

	jobject appletObj=NULL;

	Logger::alert("allocating NApplet");
	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(I)V");
		appletObj=env->NewObject(appletClass,initID,applet);
	}
	env->DeleteLocalRef(appletClass);
	Logger::alert("allocated NApplet");

	return appletObj;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_input_Input_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
//	delete applet;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_input_Input_startLogging(JNIEnv *env,jobject obj,jobject appletObj,jobject streamObj){
	Applet *applet=NULL;
	jclass appletClass=env->GetObjectClass(appletObj);
	{
		jfieldID nativeHandleID=env->GetFieldID(appletClass,"mNativeHandle","I");
		applet=(Applet*)env->GetIntField(appletObj,nativeHandleID);
	}
	env->DeleteLocalRef(appletClass);

	Stream::ptr stream=((JApplication*)((Input*)applet)->app)->makeStream(env,streamObj);

	((Input*)applet)->startLogging(stream);
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_input_Input_stopLogging(JNIEnv *env,jobject obj,jobject appletObj){
	Applet *applet=NULL;
	jclass appletClass=env->GetObjectClass(appletObj);
	{
		jfieldID nativeHandleID=env->GetFieldID(appletClass,"mNativeHandle","I");
		applet=(Applet*)env->GetIntField(appletObj,nativeHandleID);
	}
	env->DeleteLocalRef(appletClass);

	((Input*)applet)->stopLogging();
}

#endif
