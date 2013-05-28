#include "../../Particles.h"
#include <jni.h>

Applet *applet=NULL;

void Java_us_toadlet_pad(JNIEnv *env);

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_particles_Particles_createApplet(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);

	Logger::alert("getting Application");
	Application *app=NULL;
	jclass appClass=env->GetObjectClass(obj);
	{
		jmethodID getNativeHandleID=env->GetMethodID(appClass,"getNativeHandle","()I");
		app=(Application*)env->CallIntMethod(obj,getNativeHandleID);
	}
	env->DeleteLocalRef(appClass);

	Logger::alert("creating Applet");
	applet=new Particles(app);

	jobject japplet=NULL;

	Logger::alert("allocating NApplet");
	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(I)V");
		applet->retain();
		japplet=env->NewObject(appletClass,initID,applet);
	}
	env->DeleteLocalRef(appletClass);

	return japplet;
}
