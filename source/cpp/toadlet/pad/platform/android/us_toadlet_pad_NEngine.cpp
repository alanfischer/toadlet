#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

// Hack
#include <toadlet/tadpole.h>

using namespace toadlet::tadpole;

TOADLET_C_API AudioDevice *new_JAudioDevice(JNIEnv *env,jobject obj);

extern "C" {

#include "us_toadlet_pad_NEngine.h"

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_destroy(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	if(engine!=NULL){
		engine->destroy();
		delete engine;

		env->SetIntField(obj,nengineNativeHandleID,(jint)NULL);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_installHandlers(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	engine->installHandlers();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

	RenderDevice *device=NULL;
	if(deviceObj!=NULL){
		if(env->IsInstanceOf(deviceObj,nrenderDeviceClass)){
			device=(RenderDevice*)env->GetIntField(deviceObj,nrenderDeviceNativeHandleID);
		}
//		else{
//			device=new JRenderDevice(env,deviceObj);
//		}
	}
	
	engine->setRenderDevice(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setAudioDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);

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

Scene::ptr scene;
SceneRenderer::ptr sceneRenderer;
CameraNode::ptr camera;
MeshNode::ptr mesh;

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_hack_1addNodes(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);
	
	scene=Scene::ptr(new Scene(engine));
	sceneRenderer=SceneRenderer::ptr(new SceneRenderer(scene));
	scene->setSceneRenderer(sceneRenderer);
	
	camera=engine->createNodeType(CameraNode::type(),scene);
	scene->getRoot()->attach(camera);
	
//	camera->setLookAt(
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_hack_1render(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,nengineNativeHandleID);
	
	RenderDevice *device=engine->getRenderDevice();
	device->beginScene();
		camera->render(device);
	device->endScene();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_makeEngine(JNIEnv *env,jobject obj,jobject contextObj){
	Java_us_toadlet_pad(env);

	Engine *engine=new Engine(true,false);	
	
	env->SetIntField(obj,nengineNativeHandleID,(jint)engine);
	
	jobject assetManagerObj=NULL;
	jclass contextClass=env->GetObjectClass(contextObj);
	{
		jmethodID getAssetsID=env->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
		assetManagerObj=env->CallObjectMethod(contextObj,getAssetsID);
	}
	env->DeleteLocalRef(contextClass);

	AndroidAssetArchive::ptr assetArchive=AndroidAssetArchive::ptr(new AndroidAssetArchive(env,assetManagerObj));
	engine->getArchiveManager()->manage(shared_static_cast<Archive>(assetArchive));

	AndroidTextureHandler::ptr textureHandler=AndroidTextureHandler::ptr(new AndroidTextureHandler(engine->getTextureManager(),env));
	engine->getTextureManager()->setDefaultStreamer(textureHandler);
}

}
