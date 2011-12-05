#include "us_toadlet_flick_NMotionDeviceListener.h"
#include "JMotionDevice.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/flick/MotionData.h>
#include <toadlet/flick/MotionDeviceListener.h>

using namespace toadlet;
using namespace toadlet::flick;

JNIEXPORT void JNICALL Java_us_toadlet_flick_NMotionDeviceListener_motionDetected(JNIEnv *env,jobject obj,jobject dataObj){
	MotionData data;
	data.time=env->GetIntField(dataObj,timeID);
	env->GetFloatArrayRegion((jfloatArray)env->GetObjectField(dataObj,accelerationID),0,3,data.acceleration.getData());
	env->GetFloatArrayRegion((jfloatArray)env->GetObjectField(dataObj,velocityID),0,3,data.velocity.getData());

	MotionDeviceListener *listener=(MotionDeviceListener*)env->GetIntField(obj,nativeHandleID);
	listener->motionDetected(data);
}
