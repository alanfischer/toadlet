#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

using namespace toadlet::tadpole;

TOADLET_C_API AudioDevice *new_JAudioDevice(JNIEnv *env,jobject obj);

extern "C" {

#include "us_toadlet_tadpole_Engine.h"

jfieldID Engine_nativeHandle=0;
jfieldID Engine_Scene_nativeHandle=0;

void Java_us_toadlet_tadpole_Engine(JNIEnv *env){
	jclass engineClass=env->FindClass("us/toadlet/tadpole/Engine");
	{
		Engine_nativeHandle=env->GetFieldID(engineClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(engineClass);

	jclass sceneClass=env->FindClass("us/toadlet/tadpole/Scene");
	{
		Engine_Scene_nativeHandle=env->GetFieldID(sceneClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(sceneClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Engine_destroy(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,Engine_nativeHandle);

	if(engine!=NULL){
		engine->destroy();
		engine->release();

		env->SetIntField(obj,Engine_nativeHandle,(jint)NULL);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Engine_installHandlers(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,Engine_nativeHandle);

	engine->installHandlers();
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Engine_setRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,Engine_nativeHandle);

	RenderDevice *device=NULL;
	if(deviceObj!=NULL){
		jclass deviceClass=env->FindClass("us/toadlet/peeper/NGLES1RenderDevice");
		if(env->IsInstanceOf(deviceObj,deviceClass)){
			jfieldID deviceNativeHandleID=env->GetFieldID(deviceClass,"mNativeHandle","I");
			device=(RenderDevice*)env->GetIntField(deviceObj,deviceNativeHandleID);
		}
//		else{
//			device=new JRenderDevice(env,deviceObj);
//		}
		env->DeleteLocalRef(deviceClass);
	}
	
	engine->setRenderDevice(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Engine_setAudioDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,Engine_nativeHandle);

	AudioDevice *device=NULL;
	if(deviceObj!=NULL){
//		if(env->IsInstanceOf(deviceObj,nAudioDeviceClass)){
//			device=(AudioDevice*)env->GetIntField(deviceObj,nAudioDeviceNativeHandleID);
//		}
//		else{
			device=new_JAudioDevice(env,deviceObj);
//		}
	}
	
	engine->setAudioDevice(device);
}

JNIEXPORT jobject JNICALL Java_us_toadlet_tadpole_Engine_createNode(JNIEnv *env,jobject obj,jclass nodeType,jobject sceneObj){
	Engine *engine=(Engine*)env->GetIntField(obj,Engine_nativeHandle);
	Scene *scene=(sceneObj!=NULL)?(Scene*)env->GetIntField(sceneObj,Engine_Scene_nativeHandle):NULL;
	
	Node *node=NULL;
	{
		jclass classObj=env->FindClass("java/lang/Class");
		jmethodID getNameID=env->GetMethodID(classObj,"getName","()Ljava/lang/String;");
		jstring nameObj=(jstring)env->CallObjectMethod(nodeType,getNameID);

		const char *nameData=env->GetStringUTFChars(nameObj,NULL);
		String name=nameData;
		env->ReleaseStringUTFChars(nameObj,nameData);
		
		env->DeleteLocalRef(nameObj);

		/// @todo: Once the nodes are moved into the same namespace as toadlet's, then this should be cleaner
		int i=name.rfind('.');
		if(i>0){
			name="toadlet.tadpole.node"+name.substr(i,name.length());
		}
		
		node=engine->createNode(name,scene);
	}

	jobject nodeObj=NULL;
	if(node!=NULL){
		jmethodID initID=env->GetMethodID(nodeType,"<init>","(I)V");
		nodeObj=env->NewObject(nodeType,initID,node);
	}

	return nodeObj;
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Engine_makeEngine(JNIEnv *env,jobject obj,jobject contextObj){
	Java_us_toadlet_tadpole_Engine(env);

	Engine *engine=new Engine(true,false);	
	engine->retain();

	env->SetIntField(obj,Engine_nativeHandle,(jint)engine);
	
	jobject assetManagerObj=NULL;
	jclass contextClass=env->GetObjectClass(contextObj);
	{
		jmethodID getAssetsID=env->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
		assetManagerObj=env->CallObjectMethod(contextObj,getAssetsID);
	}
	env->DeleteLocalRef(contextClass);

	AndroidAssetArchive::ptr assetArchive=new AndroidAssetArchive(env,assetManagerObj);
	engine->getArchiveManager()->manageArchive(assetArchive);

	AndroidTextureHandler::ptr textureHandler=new AndroidTextureHandler(engine->getTextureManager(),env);
	engine->getTextureManager()->setDefaultStreamer(textureHandler);
}

}
