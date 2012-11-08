#include "../../src/RandIsle.h"
#include <jni.h>

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_randisle_RandIsle_createApplet(JNIEnv *env,jobject obj){
	Log::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Log::alert("creating applet");
	Applet *applet=new RandIsle(app,"");

	jobject appletObj=NULL;
	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(JZ)V");
		appletObj=env->NewObject(appletClass,initID,(jlong)applet,true);
	}
	env->DeleteLocalRef(appletClass);

	return appletObj;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_randisle_RandIsle_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
}
