#include <jni.h>

namespace toadlet{
namespace flick{

jfieldID timeID=0;
jfieldID accelerationID=0;
jfieldID velocityID=0;
jfieldID nativeHandleID=0;

}
}

using namespace toadlet::flick;

extern "C" JNIEXPORT void Java_us_toadlet_flick(JNIEnv *env){
	jclass dataClass=env->FindClass("us/toadlet/flick/MotionData");
	{
		timeID=env->GetFieldID(dataClass,"time","J");
		accelerationID=env->GetFieldID(dataClass,"acceleration","[F");
		velocityID=env->GetFieldID(dataClass,"velocity","[F");
	}
	env->DeleteLocalRef(dataClass);

	if(env->ExceptionOccurred()!=0){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}

	jclass listenerClass=env->FindClass("us/toadlet/flick/NMotionDeviceListener");
	{
		nativeHandleID=env->GetFieldID(listenerClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(listenerClass);

	if(env->ExceptionOccurred()!=0){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}
}

#include "us_toadlet_flick_NMotionDeviceListener.cpp"