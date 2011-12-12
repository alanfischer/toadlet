#include "us_toadlet_flick_NInputDeviceListener.h"
#include "JInputDevice.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/flick/InputData.h>
#include <toadlet/flick/InputDeviceListener.h>

using namespace toadlet;
using namespace toadlet::flick;

JNIEXPORT void JNICALL Java_us_toadlet_flick_NInputDeviceListener_inputDetected(JNIEnv *env,jobject obj,jobject dataObj){
	InputData data;
	data.type=env->GetIntField(dataObj,typeID);
	data.time=env->GetLongField(dataObj,timeID);
	data.valid=env->GetIntField(dataObj,validID);

	jobjectArray valuesObj=(jobjectArray)env->GetObjectField(dataObj,valuesID);
	int numValues=env->GetArrayLength(valuesObj);
	data.values.resize(numValues);
	int i;
	for(i=0;i<numValues;++i){
		env->GetFloatArrayRegion((jfloatArray)env->GetObjectArrayElement(valuesObj,i),0,4,data.values[i].getData());
	}
	
	InputDeviceListener *listener=(InputDeviceListener*)env->GetIntField(obj,nativeHandleID);
	listener->inputDetected(data);
}
