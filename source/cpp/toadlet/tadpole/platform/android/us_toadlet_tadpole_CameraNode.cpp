#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_tadpole_CameraNode.h"

jfieldID CameraNode_nativeHandle=0;

void Java_us_toadlet_tadpole_CameraNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/tadpole/Node");
	{
		CameraNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_CameraNode_setClearColor(JNIEnv *env,jobject obj,jint color){
	Java_us_toadlet_tadpole_CameraNode(env); // hack

	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);

	node->setClearColor(Vector4(color));
}

JNIEXPORT jint JNICALL Java_us_toadlet_tadpole_CameraNode_getClearColor(JNIEnv *env,jobject obj){
	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);

	return node->getClearColor().getRGBA();
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_CameraNode_setLookAt(JNIEnv *env,jobject obj,jfloatArray eyeObj,jfloatArray pointObj,jfloatArray upObj){
	Java_us_toadlet_tadpole_CameraNode(env); // hack

	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);

	float *eye=env->GetFloatArrayElements(eyeObj,NULL);
	float *point=env->GetFloatArrayElements(pointObj,NULL);
	float *up=env->GetFloatArrayElements(upObj,NULL);

	node->setLookAt(eye,point,up);
	
	env->ReleaseFloatArrayElements(eyeObj,eye,0);
	env->ReleaseFloatArrayElements(pointObj,point,0);
	env->ReleaseFloatArrayElements(upObj,up,0);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_CameraNode_setProjectionFovY(JNIEnv *env,jobject obj,float fov,float ratio,float nearDist,float farDist){
	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);

	node->setProjectionFovY(fov,ratio,nearDist,farDist);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_CameraNode_setProjectionFovX(JNIEnv *env,jobject obj,float fov,float ratio,float nearDist,float farDist){
	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);

	node->setProjectionFovX(fov,ratio,nearDist,farDist);
}
		
/// @todo: Eliminate the need to call this, or at least to specify the device, since we cheat and grab it from the Engine anyway
JNIEXPORT void JNICALL Java_us_toadlet_tadpole_CameraNode_render(JNIEnv *env,jobject obj,jobject deviceObj){
	CameraNode *node=(CameraNode*)env->GetIntField(obj,CameraNode_nativeHandle);
	RenderDevice *device=node->getEngine()->getRenderDevice();

	node->render(device);
}

}
