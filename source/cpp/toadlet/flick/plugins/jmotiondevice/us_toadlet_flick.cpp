#include <jni.h>

jfieldID timeID=0;
jfieldID accelerationID=0;
jfieldID velocityID=0;
jfieldID nativeHandleID=0;

extern "C" JNIEXPORT void Java_us_toadlet_flick(JNIEnv *env){
	jclass dataClass=env->FindClass("us/toadlet/flick/MotionData");
	{
		timeID=env->GetFieldID(dataClass,"time","I");
		accelerationID=env->GetFieldID(dataClass,"acceleration","[F");
		velocityID=env->GetFieldID(dataClass,"velocity","[F");
	}
	env->DeleteLocalRef(dataClass);

	jclass listenerClass=env->FindClass("us/toadlet/flick/NMotionDeviceListener");
	{
		nativeHandleID=env->GetFieldID(listenerClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(listenerClass);
}

#include "us_toadlet_flick_NMotionDeviceListener.cpp"