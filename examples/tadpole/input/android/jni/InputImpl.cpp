#include "../../src/Input.h"
#include <jni.h>

#if 0 && ANDROID_NDK_API_LEVEL>=9
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
	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(JZ)V");
		appletObj=env->NewObject(appletClass,initID,(jlong)applet,true);
	}
	env->DeleteLocalRef(appletClass);

	return appletObj;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_input_Input_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
}
#endif
