#include "../../Logo.h"
#include <jni.h>

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_logo_Logo_createApplet(JNIEnv *env,jobject obj){
	Logger::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Logger::alert("creating applet");
	Applet *applet=new Logo(app);

	jobject appletObj=NULL;
	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(JZ)V");
		appletObj=env->NewObject(appletClass,initID,(jlong)applet,true);
	}
	env->DeleteLocalRef(appletClass);

	return appletObj;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_logo_Logo_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
}
