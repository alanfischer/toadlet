#include "../../Logo.h"
#include <jni.h>

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_logo_Logo_createApplet(JNIEnv *env,jobject obj){
	Logger::alert("getting Application");
	Application *app=new JApplication(env,obj);

	Logger::alert("creating applet");
	Applet *applet=new Logo(app);

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

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_logo_Logo_destroyApplet(JNIEnv *env,jobject obj,jobject appletObj){
	Applet *applet=NULL;
	
	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jfieldID nativeHandleID=env->GetFieldID(appletClass,"mNativeHandle","I");
		applet=(Applet*)env->GetIntField(appletObj,nativeHandleID);
	}

	delete applet;
}
