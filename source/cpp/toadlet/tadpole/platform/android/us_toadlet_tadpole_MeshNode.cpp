#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_tadpole_MeshNode.h"

jfieldID MeshNode_nativeHandle=0;

void Java_us_toadlet_tadpole_MeshNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/tadpole/Node");
	{
		MeshNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_MeshNode_setMesh(JNIEnv *env,jobject obj,jstring nameObj){
	Java_us_toadlet_tadpole_MeshNode(env); // hack

	MeshNode *node=(MeshNode*)env->GetIntField(obj,MeshNode_nativeHandle);

	const char *nameData=env->GetStringUTFChars(nameObj,NULL);
	String name=nameData;
	env->ReleaseStringUTFChars(nameObj,nameData);

	node->setMesh(name);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_MeshNode_setAlpha(JNIEnv *env,jobject obj,jfloat alpha){
	Java_us_toadlet_tadpole_MeshNode(env); // hack

	MeshNode *node=(MeshNode*)env->GetIntField(obj,MeshNode_nativeHandle);
	
	RenderState *state=node->getSharedRenderState();
	
	MaterialState materialState;
	state->getMaterialState(materialState);
	materialState.ambient.w=alpha;
	materialState.diffuse.w=alpha;
	materialState.specular.w=alpha;
	materialState.emissive.w=alpha;
	state->setMaterialState(materialState);
	
	if(alpha<1.0f){
		state->setBlendState(BlendState(BlendState::Combination_ALPHA));
	}
	else{
		state->setBlendState(BlendState());
	}
}

}
