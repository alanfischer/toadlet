#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_pad_MeshNode.h"

jfieldID MeshNode_nativeHandle=0;

void Java_us_toadlet_pad_MeshNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/pad/Node");
	{
		MeshNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_MeshNode_setRotate(JNIEnv *env,jobject obj,jfloatArray axisObj,jfloat angle){
	MeshNode *node=(MeshNode*)env->GetIntField(obj,MeshNode_nativeHandle);

	float *axis=env->GetFloatArrayElements(axisObj,NULL);
	
	node->setRotate(axis,angle);
	
	env->ReleaseFloatArrayElements(axisObj,axis,0);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_MeshNode_setMesh(JNIEnv *env,jobject obj,jstring nameObj){
	Java_us_toadlet_pad_MeshNode(env); // hack

	MeshNode *node=(MeshNode*)env->GetIntField(obj,MeshNode_nativeHandle);

	const char *nameData=env->GetStringUTFChars(nameObj,NULL);
	String name=nameData;
	env->ReleaseStringUTFChars(nameObj,nameData);

	node->setMesh(name);
}

}
