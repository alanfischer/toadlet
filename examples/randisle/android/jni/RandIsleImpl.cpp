#include "../../src/RandIsle.h"
#include <jni.h>

using namespace randisle;

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_randisle_RandIsle_createApplet(JNIEnv *env,jobject obj){
	Log::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Log::alert("creating applet");
	Applet *applet=new RandIsle(app,"");

	jobject appletObj=NULL;
	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(JZ)V");
		applet->retain();
		appletObj=env->NewObject(appletClass,initID,(jlong)applet,true);
	}
	env->DeleteLocalRef(appletClass);

	return appletObj;
}
