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

JNIEXPORT void JNICALL Java_us_toadlet_pad_LightNode_setLightDirection(JNIEnv *env,jobject obj,jfloatArray directionObj){
	Java_us_toadlet_pad_LightNode(env); // hack

	LightNode *node=(LightNode*)env->GetIntField(obj,LightNode_nativeHandle);

	float *direction=env->GetFloatArrayElements(directionObj,NULL);

	LightState state;
	node->getLightState(state);
	state.direction=direction;
	state.specularColor=Vector4(1,1,1,1);
	node->setLightState(state);

	env->ReleaseFloatArrayElements(directionObj,direction,0);
}

}
