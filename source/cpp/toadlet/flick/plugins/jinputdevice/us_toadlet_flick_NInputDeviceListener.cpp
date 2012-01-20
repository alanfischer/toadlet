#include "JInputDevice.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/flick/InputData.h>
#include <toadlet/flick/InputDeviceListener.h>

using namespace toadlet;
using namespace toadlet::flick;

extern "C"{

#include "us_toadlet_flick_NInputDeviceListener.h"

jfieldID NInputDeviceListener_nativeHandle=0;

jfieldID NInputData_type=0;
jfieldID NInputData_time=0;
jfieldID NInputData_valid=0;
jfieldID NInputData_values=0;

void Java_us_toadlet_flick_NInputDeviceListener(JNIEnv *env){
	jclass listenerClass=env->FindClass("us/toadlet/flick/NInputDeviceListener");
	{
		NInputDeviceListener_nativeHandle=env->GetFieldID(listenerClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(listenerClass);

	jclass dataClass=env->FindClass("us/toadlet/flick/InputData");
	{
		NInputData_type=env->GetFieldID(dataClass,"type","I");
		NInputData_time=env->GetFieldID(dataClass,"time","J");
		NInputData_valid=env->GetFieldID(dataClass,"valid","I");
		NInputData_values=env->GetFieldID(dataClass,"values","[[F");
	}
	env->DeleteLocalRef(dataClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_flick_NInputDeviceListener_inputDetected(JNIEnv *env,jobject obj,jobject dataObj){
	InputData data;
	data.type=env->GetIntField(dataObj,NInputData_type);
	data.time=env->GetLongField(dataObj,NInputData_time);
	data.valid=env->GetIntField(dataObj,NInputData_valid);

	jobjectArray valuesObj=(jobjectArray)env->GetObjectField(dataObj,NInputData_values);
	int numValues=env->GetArrayLength(valuesObj);
	data.values.resize(numValues);
	int i;
	for(i=0;i<numValues;++i){
		env->GetFloatArrayRegion((jfloatArray)env->GetObjectArrayElement(valuesObj,i),0,4,data.values[i].getData());
	}
	
	InputDeviceListener *listener=(InputDeviceListener*)env->GetIntField(obj,NInputDeviceListener_nativeHandle);
	listener->inputDetected(data);
}

}