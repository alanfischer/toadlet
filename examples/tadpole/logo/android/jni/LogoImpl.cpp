#include "../../Logo.h"
#include <jni.h>

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_logo_Logo_createApplet(JNIEnv *env,jobject obj){
	Logger::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Logger::alert("creating applet");
	Applet *applet=new Logo(app);

	jobject appletObj=NULL;

	Logger::alert("APP");
	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
	Logger::alert("APP1");
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(JZ)V");
	Logger::alert(String("APP:")+(int)applet);
		appletObj=env->NewObject(appletClass,initID,(jlong)applet,true);
	Logger::alert("APP3");
	}
	env->DeleteLocalRef(appletClass);

	Logger::alert("APP3");
	return appletObj;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_logo_Logo_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
	Applet *applet=NULL;
	
/*	jclass appletClass=env->FindClass("us/toadlet/pad/Applet");
	{
		jfieldID nativeHandleID=env->GetFieldID(appletClass,"mNativeHandle","I");
		applet=(Applet*)env->GetIntField(appletObj,nativeHandleID);
	}
*/
	delete applet;
}
