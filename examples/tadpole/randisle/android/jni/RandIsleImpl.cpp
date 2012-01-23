#include "../../src/RandIsle.h"
#include <jni.h>

#if ANDROID_NDK_API_LEVEL>=9
extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *activity,void *savedState,size_t savedStateSize){
	Logger::alert("ANativeActivity_onCreate");

	AndroidApplication *app=new AndroidApplication();
	app->setApplet(new RandIsle(app,""));
	app->setNativeActivity(activity);
}
#else
extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_randisle_RandIsle_createApplet(JNIEnv *env,jobject obj){
	Logger::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Logger::alert("creating applet");
	Applet *applet=new RandIsle(app,"");

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

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_randisle_RandIsle_destroyApplet(JNIEnv *env,jobject obj,jobject applet){
//	delete applet;
}
#endif
