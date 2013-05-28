#include "../../src/RandIsle.h"
#include <toadlet/pad/platform/android/AndroidApplication.h>
#include <jni.h>
#include <android/native_activity.h>

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *activity,void *savedState,size_t savedStateSize){
	Log::alert("ANativeActivity_onCreate");

	AndroidApplication *app=new AndroidApplication();
	app->setApplet(new RandIsle(app,""));
	app->setNativeActivity(activity);
}
