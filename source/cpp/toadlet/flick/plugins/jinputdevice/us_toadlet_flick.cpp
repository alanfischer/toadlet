#include <jni.h>

namespace toadlet{
namespace flick{

jfieldID typeID=0;
jfieldID timeID=0;
jfieldID validID=0;
jfieldID valuesID=0;
jfieldID nativeHandleID=0;

}
}

using namespace toadlet::flick;

extern "C" JNIEXPORT void Java_us_toadlet_flick(JNIEnv *env){
	jclass dataClass=env->FindClass("us/toadlet/flick/InputData");
	{
		typeID=env->GetFieldID(dataClass,"type","I");
		timeID=env->GetFieldID(dataClass,"time","J");
		validID=env->GetFieldID(dataClass,"valid","I");
		valuesID=env->GetFieldID(dataClass,"values","[[F");
	}
	env->DeleteLocalRef(dataClass);

	if(env->ExceptionOccurred()!=0){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}

	jclass listenerClass=env->FindClass("us/toadlet/flick/NInputDeviceListener");
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

#include "us_toadlet_flick_NInputDeviceListener.cpp"