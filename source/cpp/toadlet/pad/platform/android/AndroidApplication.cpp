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
#include <toadlet/egg/System.h>
#include <toadlet/egg/platform/android/AndroidAssetArchive.h>
#include <toadlet/tadpole/platform/android/AndroidTextureStreamer.h>
#include <android/sensor.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

TOADLET_C_API RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
TOADLET_C_API RenderDevice *new_GLES1RenderDevice();
TOADLET_C_API RenderDevice *new_GLES2RenderDevice();
TOADLET_C_API InputDevice *new_AndroidSensorDevice(int type);

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
	mFormat->setDepthBits(16);
	mFormat->setFlags(2);
	#if defined(TOADLET_DEBUG)
		mFormat->setDebug(true);
	#else
		mFormat->setDebug(false);
	#endif
}

AndroidApplication::~AndroidApplication(){
}

bool AndroidApplication::create(String renderDevice,String audioDevice){
	mConfig=AConfiguration_new();

	JNIEnv *env=mActivity->env;
	jobject obj=mActivity->clazz;

	mEngine=new Engine(env,obj);
	
	mEngine->installHandlers();

	AndroidTextureStreamer::ptr textureStreamer=new AndroidTextureStreamer(mEngine->getTextureManager(),env);
	mEngine->getTextureManager()->setDefaultStreamer(textureStreamer);
	
	InputDevice::ptr linearDevice=new_AndroidSensorDevice(ASENSOR_TYPE_ACCELEROMETER);
	if(linearDevice->create()){
		mInputDevices[linearDevice->getType()]=linearDevice;
	}

	InputDevice::ptr angularDevice=new_AndroidSensorDevice(ASENSOR_TYPE_GYROSCOPE);
	if(angularDevice->create()){
		mInputDevices[angularDevice->getType()]=angularDevice;
	}

	InputDevice::ptr lightDevice=new_AndroidSensorDevice(ASENSOR_TYPE_LIGHT);
	if(lightDevice->create()){
		mInputDevices[lightDevice->getType()]=lightDevice;
	}

	InputDevice::ptr proximityDevice=new_AndroidSensorDevice(ASENSOR_TYPE_PROXIMITY);
	if(proximityDevice->create()){
		mInputDevices[proximityDevice->getType()]=proximityDevice;
	}

	InputDevice::ptr magneticDevice=new_AndroidSensorDevice(ASENSOR_TYPE_MAGNETIC_FIELD);
	if(magneticDevice->create()){
		mInputDevices[magneticDevice->getType()]=magneticDevice;
	}

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

	mEngine=NULL;

	int i;
	for(i=0;i<InputDevice::InputType_MAX;++i){
		if(mInputDevices[i]!=NULL){
			mInputDevices[i]->destroy();
			mInputDevices[i]=NULL;
		}
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
	// pthread_join on android currently hangs here
//	mThread->join();
	mThread=NULL;
}

bool AndroidApplication::isRunning() const{
	return mRun;
}

void AndroidApplication::notifyWindowCreated(ANativeWindow *window){
	Log::alert("notifyWindowCreated");

	mWindowMutex.lock();
		if(mRun){
			mNotifyWindowCreated=window;
			mWindowCondition.wait(&mWindowMutex);
		}
		else{
			windowCreated(window);
		}
	mWindowMutex.unlock();
}

void AndroidApplication::notifyWindowDestroyed(ANativeWindow *window){
	Log::alert("notifyWindowDestroyed");

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
	Log::alert("onNativeWindowResized");

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
	Log::alert("notifyQueueCreated");

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
	Log::alert("notifyQueueDestroyed");

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
	activity->callbacks->onConfigurationChanged = onConfigurationChanged;
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
	if(mQueue!=NULL){
		AInputQueue_attachLooper(mQueue,mLooper,0,NULL,this);
	}
	
	uint64 lastTime=System::mtime();
	while(mRun){
		stepEventLoop();
	
		uint64 currentTime=System::mtime();
		int dt=currentTime-lastTime;

		update(dt);
		
		if(mRenderDevice!=NULL){
			// onNativeWindowResized doesn't appear to be called properly, so instead we check each frame for a resize
			if(mRenderTarget->getWidth()!=mWidth || mRenderTarget->getHeight()!=mHeight){
				windowResized();
			}
		
			if(mRenderDevice->activate()==RenderDevice::DeviceStatus_NEEDSRESET){
				mEngine->contextReset(mRenderDevice);
			}

			render();
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

	if(mQueue!=NULL){
		AInputQueue_detachLooper(mQueue);
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
					int key=translateKey(AKeyEvent_getKeyCode(event));
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

	int nativeFormat=ANativeWindow_getFormat(window);
	if(nativeFormat==WINDOW_FORMAT_RGB_565){
		mFormat->setPixelFormat(TextureFormat::Format_RGB_5_6_5);
	}
	else{
		mFormat->setPixelFormat(TextureFormat::Format_RGBA_8);
	}

	// Start with gles2 and then try gles
	for(;mFormat->getFlags()>=0 && mRenderDevice==NULL;mFormat->setFlags(mFormat->getFlags()-2)){
		RenderTarget::ptr target;
		TOADLET_TRY
			target=new_EGLWindowRenderTarget(0,mWindow,mFormat);
		TOADLET_CATCH_ANONYMOUS(){target=NULL;}
		if(target!=NULL && target->isValid()==false){
			target->destroy();
			target=NULL;
		}

		RenderDevice::ptr device;
		if(target!=NULL){
			TOADLET_TRY
				if(mFormat->getFlags()==2){
					device=new_GLES2RenderDevice();
				}
				else{
					device=new_GLES1RenderDevice();
				}
			TOADLET_CATCH_ANONYMOUS(){device=NULL;}
			if(device!=NULL && device->create(target,0)==false){
				device->destroy();
				target->destroy();
				device=NULL;
				target=NULL;
			}
		}
		
		mRenderTarget=target;
		mRenderDevice=device;
	}
	
	if(mRenderDevice!=NULL){
		mEngine->setRenderDevice(mRenderDevice);
	}
}

void AndroidApplication::windowDestroyed(ANativeWindow *window){
	if(mRenderDevice!=NULL){
		mEngine->setRenderDevice(NULL);
		mRenderDevice->destroy();
		mRenderDevice=NULL;
	}

	if(mRenderTarget!=NULL){
		mRenderTarget->destroy();
		mRenderTarget=NULL;
	}

	mWindow=NULL;
}

void AndroidApplication::windowResized(){
	if(mRenderTarget!=NULL && mRenderDevice!=NULL){
		mWidth=mRenderTarget->getWidth();
		mHeight=mRenderTarget->getHeight();
		resized(mWidth,mHeight);

		render();
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

int AndroidApplication::translateKey(int key){
	switch(key){
		case AKEYCODE_SOFT_LEFT:
			return Key_SOFTLEFT;
		case AKEYCODE_SOFT_RIGHT:
			return Key_SOFTRIGHT;
		case AKEYCODE_BACK:
			return Key_BACK;
		case AKEYCODE_STAR:
			return '*';
		case AKEYCODE_POUND:
			return '#';
		case AKEYCODE_DPAD_UP:
			return Key_UP;
		case AKEYCODE_DPAD_DOWN:
			return Key_DOWN;
		case AKEYCODE_DPAD_LEFT:
			return Key_LEFT;
		case AKEYCODE_DPAD_RIGHT:
			return Key_RIGHT;
		case AKEYCODE_DPAD_CENTER:
			return Key_ACTION;
		case AKEYCODE_COMMA:
			return ',';
		case AKEYCODE_PERIOD:
			return '.';
		case AKEYCODE_TAB:
			return Key_TAB;
		case AKEYCODE_SPACE:
			return Key_SPACE;
		case AKEYCODE_ENTER:
			return Key_ENTER;
		case AKEYCODE_SHIFT_LEFT:
		case AKEYCODE_SHIFT_RIGHT:
			return Key_SHIFT;
		case AKEYCODE_ALT_LEFT:
		case AKEYCODE_ALT_RIGHT:
			return Key_ALT;
		case AKEYCODE_DEL:
			return Key_DELETE;
		case AKEYCODE_SYM:
			return Key_SPECIAL;
	}

	if(key>=AKEYCODE_0 && key<=AKEYCODE_9){
		return key-AKEYCODE_0+'0';
	}
	else if(key>=AKEYCODE_A && key<=AKEYCODE_Z){
		return key-AKEYCODE_A+'A';
	}

	return key;
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

void AndroidApplication::onConfigurationChanged(ANativeActivity *activity){
}

}
}
