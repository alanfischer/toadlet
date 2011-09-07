#include "../../Particles.h"
#include <jni.h>

JAndroidApplication *application=NULL;
Applet *applet=NULL;

extern "C" JNIEXPORT jobject JNICALL Java_us_toadlet_particles_Particles_createApplet(JNIEnv *env,jobject obj,jobject app){
	Logger::alert("Creating application");
	application=new JAndroidApplication(env,app);

	Logger::alert("Creating applet");
	applet=new Particles(application);

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

extern "C" JNIEXPORT void JNICALL Java_us_toadlet_particles_Particles_destroyApplet(JNIEnv *env,jobject obj,jobject applet){
	delete applet;
	delete application;
}
