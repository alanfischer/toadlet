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

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

TOADLET_C_API RenderDevice* new_GLRenderDevice();
TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *env,jobject obj);

namespace toadlet{
namespace pad{

AndroidApplication::AndroidApplication():
	mActivity(NULL),
	mWindow(NULL),
	//mThread,
	mRunning(false),

	mEngine(NULL),
	mRenderDevice(NULL),
	mAudioDevice(NULL),
	mApplet(NULL)
{
}

AndroidApplication::~AndroidApplication(){
}

bool AndroidApplication::create(String renderDevice,String audioDevice,String motionDevice,String joyDevice){
	mEngine=new Engine(true);
	
	mEngine->installHandlers();

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
}

void AndroidApplication::start(){
	mRunning=true;
	mThread=Thread::ptr(new Thread(this));
	mThread->start();
}

void AndroidApplication::stop(){
	mRunning=false;
	mThread->join();
	mThread=NULL;
}

bool AndroidApplication::isRunning() const{
	return mRunning;
}

void AndroidApplication::nativeWindowCreated(ANativeWindow *window){
	mWindow=window;
}

void AndroidApplication::nativeWindowDestroyed(ANativeWindow *window){
	mWindow=NULL;
}
	
int AndroidApplication::getWidth() const{
	return (mWindow!=NULL)?ANativeWindow_getWidth(mWindow):0;
}

int AndroidApplication::getHeight() const{
	return (mWindow!=NULL)?ANativeWindow_getHeight(mWindow):0;
}

void AndroidApplication::setDifferenceMouse(bool difference){
}

bool AndroidApplication::getDifferenceMouse() const{
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
//	activity->callbacks->onInputQueueCreated = onInputQueueCreated;
//	activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;

	mActivity=activity;

	create("","","","");
}

void AndroidApplication::run(){
	while(mRunning!=NULL){
		System::msleep(0);
	}
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
	app->nativeWindowCreated(window);
}

void AndroidApplication::onNativeWindowDestroyed(ANativeActivity *activity,ANativeWindow *window){
	AndroidApplication *app=(AndroidApplication*)activity->instance;
	app->nativeWindowDestroyed(window);
}

}
}
