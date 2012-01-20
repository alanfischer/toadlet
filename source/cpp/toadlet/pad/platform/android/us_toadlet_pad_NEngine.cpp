#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

// Hack
#include <toadlet/tadpole.h>

using namespace toadlet::tadpole;

TOADLET_C_API AudioDevice *new_JAudioDevice(JNIEnv *env,jobject obj);

extern "C" {

#include "us_toadlet_pad_NEngine.h"

jfieldID NEngine_nativeHandle=0;

void Java_us_toadlet_pad_NEngine(JNIEnv *env){
	jclass engineClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		NEngine_nativeHandle=env->GetFieldID(engineClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(engineClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_destroy(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);

	if(engine!=NULL){
		engine->destroy();
		delete engine;

		env->SetIntField(obj,NEngine_nativeHandle,(jint)NULL);
	}
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_installHandlers(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);

	engine->installHandlers();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setRenderDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);

	RenderDevice *device=NULL;
	if(deviceObj!=NULL){
		jclass deviceClass=env->FindClass("us/toadlet/peeper/NGLRenderDevice");
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

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_setAudioDevice(JNIEnv *env,jobject obj,jobject deviceObj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);

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
LightNode::ptr light;

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_hack_1addNodes(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);
	
	scene=Scene::ptr(new Scene(engine));
	sceneRenderer=SceneRenderer::ptr(new SceneRenderer(scene));
	scene->setSceneRenderer(sceneRenderer);
	
	camera=engine->createNodeType(CameraNode::type(),scene);
	scene->getRoot()->attach(camera);

	camera->setLookAt(Vector3(0,-4,0),Vector3(0,0,0),Vector3(0,0,1));
	
	mesh=engine->createNodeType(MeshNode::type(),scene);
	scene->getRoot()->attach(mesh);
	
	mesh->setMesh("crank.tmsh");
	mesh->getController()->start();
	mesh->getController()->setCycling(Controller::Cycling_REFLECT);
	
	light=engine->createNodeType(LightNode::type(),scene);
	scene->getRoot()->attach(light);

	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction.set(Math::X_UNIT_VECTOR3);
	state.diffuseColor.set(Math::ONE_VECTOR4);
	state.specularColor.set(Math::ONE_VECTOR4);
	light->setLightState(state);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_hack_1render(JNIEnv *env,jobject obj){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);
		
	RenderDevice *device=engine->getRenderDevice();

	RenderTarget *target=device->getRenderTarget();
	int width=target->getWidth(),height=target->getHeight();
	if(camera!=NULL && width>0 && height>0){
		if(width>=height){
			scalar ratio=Math::div(Math::fromInt(width),Math::fromInt(height));
			camera->setProjectionFovY(Math::degToRad(Math::fromInt(75)),ratio,camera->getNearDist(),camera->getFarDist());
		}
		else{
			scalar ratio=Math::div(Math::fromInt(height),Math::fromInt(width));
			camera->setProjectionFovX(Math::degToRad(Math::fromInt(75)),ratio,camera->getNearDist(),camera->getFarDist());
		}
	}

	device->beginScene();
		camera->render(device);
	device->endScene();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_hack_1update(JNIEnv *env,jobject obj,jint dt){
	Engine *engine=(Engine*)env->GetIntField(obj,NEngine_nativeHandle);

	scene->update(dt);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NEngine_makeEngine(JNIEnv *env,jobject obj,jobject contextObj){
	Engine *engine=new Engine(true,false);	
	
	env->SetIntField(obj,NEngine_nativeHandle,(jint)engine);
	
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
