#include <toadlet/tadpole/Scene.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_pad_Scene.h"

jfieldID Scene_Engine_nativeHandle=0;
jfieldID Scene_nativeHandle=0;

void Java_us_toadlet_pad_Scene(JNIEnv *env){
	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		Scene_Engine_nativeHandle=env->GetFieldID(engineClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(engineClass);

	jclass sceneClass=env->FindClass("us/toadlet/pad/Scene");
	{
		Scene_nativeHandle=env->GetFieldID(sceneClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(sceneClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_Scene_destroy(JNIEnv *env,jobject obj){
	Scene *scene=(Scene*)env->GetIntField(obj,Scene_nativeHandle);

	if(scene!=NULL){
		scene->destroy();
		delete scene;

		env->SetIntField(obj,Scene_nativeHandle,(jint)NULL);
	}
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_Scene_getRoot(JNIEnv *env,jobject obj){
	Scene *scene=(Scene*)env->GetIntField(obj,Scene_nativeHandle);
	
	ParentNode *node=scene->getRoot();
	
	jclass nodeType=env->FindClass("us/toadlet/pad/ParentNode");
	jmethodID initID=env->GetMethodID(nodeType,"<init>","(I)V");
	jobject nodeObj=env->NewObject(nodeType,initID,node);

	return nodeObj;
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_Scene_update(JNIEnv *env,jobject obj,jint dt){
	Scene *scene=(Scene*)env->GetIntField(obj,Scene_nativeHandle);

	scene->update(dt);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_Scene_makeScene(JNIEnv *env,jobject obj,jobject engineObj){
	Java_us_toadlet_pad_Scene(env);

	Engine *engine=(Engine*)env->GetIntField(engineObj,Scene_Engine_nativeHandle);

	Scene *scene=new Scene(engine);

	env->SetIntField(obj,Scene_nativeHandle,(jint)scene);
}

}
