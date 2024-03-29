#include "../../MyPlanet.h"
#include <jni.h>

JAndroidApplication *application=NULL;
Applet *applet=NULL;

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_myplanet_MyPlanet_createApplet(JNIEnv *env,jobject obj,jobject app){
	Logger::alert("Creating application");
	application=new JAndroidApplication(env,app);

	Logger::alert("Creating applet");
	applet=new MyPlanet(application);

	jobject japplet=NULL;

	Logger::alert("allocating NApplet");
	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(I)V");
		applet->retain();
		japplet=env->NewObject(appletClass,initID,applet);
	}
	env->DeleteLocalRef(appletClass);
	Logger::alert("allocated NApplet");

	return japplet;
}
