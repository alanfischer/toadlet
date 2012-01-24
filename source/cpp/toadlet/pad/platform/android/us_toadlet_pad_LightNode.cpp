#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_pad_LightNode.h"

jfieldID LightNode_nativeHandle=0;

void Java_us_toadlet_pad_LightNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/pad/Node");
	{
		LightNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_LightNode_setLightState(JNIEnv *env,jobject obj,jboolean enabled){
	Java_us_toadlet_pad_LightNode(env); // hack

	LightNode *node=(LightNode*)env->GetIntField(obj,LightNode_nativeHandle);

	LightState state;
	node->getLightState(state);
	state.direction=Vector3(1,0,0);
	node->setLightState(state);
}

}
