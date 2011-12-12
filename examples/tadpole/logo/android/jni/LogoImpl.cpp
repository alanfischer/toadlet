#include "../../Logo.h"
#include <jni.h>

extern "C" JNIEXPORT void Java_us_toadlet_pad(JNIEnv *env);

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_logo_Logo_createApplet(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);

	Logger::alert("getting Application");
	Application *app=NULL;
	jclass appClass=env->GetObjectClass(obj);
	{
		jmethodID getNativeHandleID=env->GetMethodID(appClass,"getNativeHandle","()I");
		app=(Application*)env->CallIntMethod(obj,getNativeHandleID);
	}
	env->DeleteLocalRef(appClass);

	Logger::alert("creating applet");
	Applet *applet=new Logo(app);

	jobject japplet=NULL;

	Logger::alert("allocating NApplet");
	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(I)V");
		japplet=env->NewObject(appletClass,initID,applet);
	}
	env->DeleteLocalRef(appletClass);
	Logger::alert("allocated NApplet");

	return japplet;
}

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_logo_Logo_destroyApplet(JNIEnv *env,jobject obj,jobject applet){
//	delete applet;
}
