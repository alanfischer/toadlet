#include "../../Particles.h"
#include <jni.h>

JAndroidApplication *application=NULL;
Applet *applet=NULL;

JNIEXPORT jobject JNICALL Java_us_toadlet_particles_Particles_createApplet(JNIEnv *env,jobject obj,jobject app){
	application=new JAndroidApplication(env,app);
	applet=new Particles(application);
	jobject japplet=NULL;

	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		jmethodID initID=env->GetMethodID(appletClass,"<init>","(L)V");
		japplet=env->NewObject(appletClass,initID,applet);
	}
	env->DeleteLocalRef(appletClass);

	return japplet;
}

JNIEXPORT void JNICALL Java_us_toadlet_particles_Particles_destroyApplet(JNIEnv *env,jobject obj,jobject applet){
	delete applet;
	delete application;
}
