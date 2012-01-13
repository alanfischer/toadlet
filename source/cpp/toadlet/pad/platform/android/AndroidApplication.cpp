/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include "AndroidApplication.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

TOADLET_C_API RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
TOADLET_C_API RenderDevice *new_GLRenderDevice();
TOADLET_C_API AudioDevice *new_JAudioDevice(JNIEnv *env,jobject obj);

namespace toadlet{
namespace pad{

AndroidApplication::AndroidApplication():
	mActivity(NULL),
	mConfig(NULL),
	mLooper(NULL),
	mQueue(NULL),
	mWindow(NULL),mNotifyWindowCreated(NULL),mNotifyWindowDestroyed(NULL),
	mNotifyWindowResized(false),
	mNotifyQueueCreated(NULL),mNotifyQueueDestroyed(NULL),
	mWidth(0),mHeight(0),
	mLastX(0),mLastY(0),
	mDifferenceMouse(false),
	//mThread,
	mRun(false),

	mEngine(NULL),
	mRenderDevice(NULL),
	mAudioDevice(NULL),
	mApplet(NULL)
{
	mFormat=WindowRenderTargetFormat::ptr(new WindowRenderTargetFormat());
	mFormat->pixelFormat=TextureFormat::Format_RGB_5_6_5;
	mFormat->depthBits=16;
	mFormat->multisamples=0;
	mFormat->threads=0;
	#if defined(TOADLET_DEBUG)
		mFormat->debug=true;
	#else
		mFormat->debug=false;
	#endif
}

AndroidApplication::~AndroidApplication(){
}

bool AndroidApplication::create(String renderDevice,String audioDevice){
	mConfig=AConfiguration_new();

	mEngine=new Engine(true);
	
	mEngine->installHandlers();

	JNIEnv *env=mActivity->env;
	jobject obj=mActivity->clazz;
	
	jobject assetManagerObj=NULL;
	jclass contextClass=env->FindClass("android/content/Context");
	{
		jmethodID getAssetsID=env->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
		assetManagerObj=env->CallObjectMethod(obj,getAssetsID);
	}
	env->DeleteLocalRef(contextClass);

	AndroidAssetArchive::ptr assetArchive=AndroidAssetArchive::ptr(new AndroidAssetArchive(env,assetManagerObj));
	mEngine->getArchiveManager()->manage(shared_static_cast<Archive>(assetArchive));

	AndroidTextureHandler::ptr textureHandler=AndroidTextureHandler::ptr(new AndroidTextureHandler(mEngine->getTextureManager(),env));
	mEngine->getTextureManager()->setDefaultStreamer(textureHandler);
	
	if(mApplet!=NULL){
		mApplet->create();
	}

	activate();
}

void AndroidApplication::destroy(){
	if(mEngine!=NULL){
		mEngine->destroy();
	}

	deactivate();

	if(mApplet!=NULL){
		mApplet->destroy();
		mApplet=NULL;
	}

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}

	if(mQueue!=NULL){
        AInputQueue_detachLooper(mQueue);
		mQueue=NULL;
	}
	
	if(mConfig!=NULL){
		AConfiguration_delete(mConfig);
		mConfig=NULL;
	}
	
	mLooper=NULL;
}

void AndroidApplication::start(){
	mRun=true;
	mThread=Thread::ptr(new Thread(this));
	mThread->start();
}

void AndroidApplication::stop(){
	mRun=false;
	mThread->join();
	mThread=NULL;
}

bool AndroidApplication::isRunning() const{
	return mRun;
}

void AndroidApplication::notifyWindowCreated(ANativeWindow *window){
	Logger::alert("notifyWindowCreated");

	mWindowMutex.lock();
		if(mRun){
			mNotifyWindowCreated=window;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			windowCreated(window);
		}
	mWindowMutex.unlock();
	
	notifyWindowResized();
}

void AndroidApplication::notifyWindowDestroyed(ANativeWindow *window){
	Logger::alert("notifyWindowDestroyed");

	mWindowMutex.lock();
		if(mRun){
			mNotifyWindowDestroyed=window;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			windowDestroyed(window);
		}
	mWindowMutex.unlock();
}

void AndroidApplication::notifyWindowResized(){
	Logger::alert("onNativeWindowResized");

	mWindowMutex.lock();
		if(mRun){
			mNotifyWindowResized=true;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			windowResized();
		}
	mWindowMutex.unlock();
}

void AndroidApplication::notifyQueueCreated(AInputQueue *queue){
	Logger::alert("notifyQueueCreated");

	mWindowMutex.lock();
		if(mRun){
			mNotifyQueueCreated=queue;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			queueCreated(queue);
		}
	mWindowMutex.unlock();
}

void AndroidApplication::notifyQueueDestroyed(AInputQueue *queue){
	Logger::alert("notifyQueueDestroyed");

	mWindowMutex.lock();
		if(mRun){
			mNotifyQueueDestroyed=queue;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			queueDestroyed(queue);
		}
	mWindowMutex.unlock();
}

void AndroidApplication::setNativeActivity(ANativeActivity *activity){
	activity->instance=this;

	activity->callbacks->onDestroy = onDestroy;
	activity->callbacks->onStart = onStart;
//	activity->callbacks->onResume = onResume;
//	activity->callbacks->onSaveInstanceState = onSaveInstanceState;
//	activity->callbacks->onPause = onPause;
	activity->callbacks->onStop = onStop;
//	activity->callbacks->onConfigurationChanged = onConfigurationChanged;
//	activity->callbacks->onLowMemory = onLowMemory;
//	activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
	activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
	activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
	activity->callbacks->onNativeWindowResized = onNativeWindowResized;
	activity->callbacks->onInputQueueCreated = onInputQueueCreated;
	activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;

	mActivity=activity;

	create("","");
}

void AndroidApplication::run(){
	mLooper=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

	uint64 lastTime=System::mtime();
	while(mRun){
		stepEventLoop();
	
		uint64 currentTime=System::mtime();
		int dt=currentTime-lastTime;

		update(dt);

		if(mRenderDevice!=NULL){
			if(mRenderDevice->activate()==RenderDevice::DeviceStatus_NEEDSRESET){
				mEngine->contextReset(mRenderDevice);
			}

			render(mRenderDevice);
		}

		if(mAudioDevice!=NULL){
			mAudioDevice->update(dt);
		}

		mWindowMutex.lock();
			if(mNotifyWindowCreated!=NULL){
				windowCreated(mNotifyWindowCreated);
				
				mNotifyWindowCreated=NULL;
				mWindowCondition.notify();
			}
			if(mNotifyWindowDestroyed!=NULL){
				windowDestroyed(mNotifyWindowDestroyed);
				
				mNotifyWindowDestroyed=NULL;
				mWindowCondition.notify();
			}
			if(mNotifyWindowResized){
				windowResized();
				
				mNotifyWindowResized=false;
				mWindowCondition.notify();
			}
			if(mNotifyQueueCreated!=NULL){
				queueCreated(mNotifyQueueCreated);
				
				mNotifyQueueCreated=NULL;
				mWindowCondition.notify();
			}
			if(mNotifyQueueDestroyed!=NULL){
				queueDestroyed(mNotifyQueueDestroyed);

				mNotifyQueueDestroyed=NULL;
				mWindowCondition.notify();
			}
		mWindowMutex.unlock();

		lastTime=currentTime;

		System::msleep(0);
	}

	mLooper=NULL;
}

void AndroidApplication::stepEventLoop(){
	if(mQueue==NULL){
		return;
	}

	int events=0;
	while(ALooper_pollAll(0,NULL,&events,NULL)>=0){
		AInputEvent *event=NULL;
		if(AInputQueue_getEvent(mQueue,&event)>=0){
			if(AInputQueue_preDispatchEvent(mQueue,event)){
				return;
			}
			int32_t handled=0;

			switch(AInputEvent_getType(event)){
				case AINPUT_EVENT_TYPE_KEY:{
					int key=0;
					switch(AKeyEvent_getKeyCode(event)){
						case AKEYCODE_SPACE:
							key=' ';
						break;
					}
					
					int action=AKeyEvent_getAction(event);
					if(action==AKEY_EVENT_ACTION_DOWN){
						keyPressed(key);
					}
					else if(action==AKEY_EVENT_ACTION_UP){
						keyReleased(key);
					}
				} break;
				case AINPUT_EVENT_TYPE_MOTION:{
					int x=AMotionEvent_getX(event,0),y=AMotionEvent_getY(event,0);
					int action=AMotionEvent_getAction(event);
					if(action==AMOTION_EVENT_ACTION_DOWN){
						mousePressed(x,y,0);
					}
					else if(action==AMOTION_EVENT_ACTION_MOVE){
						if(mDifferenceMouse){
							mouseMoved(mLastX-x,mLastY-y);
						}
						else{
							mouseMoved(x,y);
						}
					}
					else if(action==AMOTION_EVENT_ACTION_UP || action==AMOTION_EVENT_ACTION_CANCEL){
						if(mLastX!=x || mLastY!=y){
							if(mDifferenceMouse){
								mouseMoved(mLastX-x,mLastY-y);
							}
							else{
								mouseMoved(x,y);
							}
						}
						mouseReleased(x,y,0);
					}
					mLastX=x;mLastY=y;
				} break;
			}
			
			AInputQueue_finishEvent(mQueue,event,handled);
		}
	}
}

void AndroidApplication::windowCreated(ANativeWindow *window){
	mWindow=window;

	RenderTarget *target=NULL;
	TOADLET_TRY
		target=new_EGLWindowRenderTarget(0,mWindow,mFormat);
	TOADLET_CATCH(const Exception &){target=NULL;}
	
	if(target!=NULL && target->isValid()==false){
		delete target;
		target=NULL;
	}
	mRenderTarget=target;
	
//	ANativeWindow_setBuffersGeometry(mWindow,mRenderTarget->getWidth(),mRenderTarget->getHeight(),0);

	RenderDevice *device=NULL;
	if(target!=NULL){
		device=new_GLRenderDevice();
		if(device->create(target,NULL)==false){
			delete device;
			device=NULL;
		}
	}
	
	if(device!=NULL){
		mRenderDevice=device;
		mRenderDevice->setRenderTarget(this);
		mEngine->setRenderDevice(mRenderDevice);
	}
}

void AndroidApplication::windowDestroyed(ANativeWindow *window){
	if(mRenderDevice!=NULL){
		mEngine->setRenderDevice(NULL);
		mRenderDevice->destroy();
		delete mRenderDevice;
		mRenderDevice=NULL;
	}

	if(mRenderTarget!=NULL){
		mRenderTarget->destroy();
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	mWindow=NULL;
}

void AndroidApplication::windowResized(){
Logger::alert(String("SIZE:")+mRenderTarget->getWidth()+","+mRenderTarget->getHeight());
	if(mRenderDevice!=NULL){
//		ANativeWindow_setBuffersGeometry(mWindow,mRenderTarget->getWidth(),mRenderTarget->getHeight(),0);

		mWidth=mRenderTarget->getWidth();
		mHeight=mRenderTarget->getHeight();
		resized(mWidth,mHeight);

		render(mRenderDevice);
	}
}

void AndroidApplication::queueCreated(AInputQueue *queue){
	mQueue=queue;
	AInputQueue_attachLooper(mQueue,mLooper,0,NULL,this);
}

void AndroidApplication::queueDestroyed(AInputQueue *queue){
	AInputQueue_detachLooper(mQueue);
	mQueue=NULL;
}

void AndroidApplication::onDestroy(ANativeActivity *activity){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->destroy();
}

void AndroidApplication::onStart(ANativeActivity *activity){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->start();
}

void AndroidApplication::onStop(ANativeActivity *activity){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->stop();
}

void AndroidApplication::onNativeWindowCreated(ANativeActivity *activity,ANativeWindow *window){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->notifyWindowCreated(window);
}

void AndroidApplication::onNativeWindowDestroyed(ANativeActivity *activity,ANativeWindow *window){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->notifyWindowDestroyed(window);
}

void AndroidApplication::onNativeWindowResized(ANativeActivity *activity,ANativeWindow *window){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->notifyWindowResized();
}

void AndroidApplication::onInputQueueCreated(ANativeActivity *activity,AInputQueue *queue){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->notifyQueueCreated(queue);
}

void AndroidApplication::onInputQueueDestroyed(ANativeActivity *activity,AInputQueue *queue){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->notifyQueueDestroyed(queue);
}

}
}
