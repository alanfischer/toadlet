#include "JApplication.h"
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

using namespace toadlet::pad;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::handler;

extern "C" {

#include "us_toadlet_pad_AndroidApplication.h"

JNIEXPORT void JNICALL Java_us_toadlet_pad_AndroidApplication_installHandlers(JNIEnv *env,jobject obj,jobject engineObj){
	Java_us_toadlet_pad(env);

	Engine *engine=(Engine*)env->GetIntField(engineObj,nengineNativeHandleID);

	jobject assetManagerObj=NULL;
	jclass contextClass=env->GetObjectClass(obj);
	{
		jmethodID getAssetsID=env->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
		assetManagerObj=env->CallObjectMethod(obj,getAssetsID);
	}
	env->DeleteLocalRef(contextClass);

	AndroidAssetArchive::ptr assetArchive=AndroidAssetArchive::ptr(new AndroidAssetArchive(env,assetManagerObj));
	engine->getArchiveManager()->manage(shared_static_cast<Archive>(assetArchive));

	AndroidTextureHandler::ptr textureHandler=AndroidTextureHandler::ptr(new AndroidTextureHandler(engine->getTextureManager(),env));
	engine->getTextureManager()->setDefaultStreamer(textureHandler);
}

}
