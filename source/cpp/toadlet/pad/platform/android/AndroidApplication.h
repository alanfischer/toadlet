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

#ifndef TOADLET_PAD_ANDROIDAPPLICATION_H
#define TOADLET_PAD_ANDROIDAPPLICATION_H

#include <toadlet/egg/Thread.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/pad/Application.h>
#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>

namespace toadlet{
namespace pad{

/// @todo: There is still a problem where the native window doesn't resize correctly on rotation
/// @todo: And the rest of the input keys need to be handled besides space
class AndroidApplication:protected Object,public Application,public Runnable{
public:
	TOADLET_OBJECT(AndroidApplication);

	AndroidApplication();
	virtual ~AndroidApplication();

	bool create(String renderDevice,String audioDevice);
	void destroy();

	void setApplet(Applet *applet){mApplet=applet;}
	Applet *getApplet() const{return mApplet;}

	void start();
	void stop();
	bool isRunning() const;

	void activate(){}
	void deactivate(){}
	bool isActive() const{return true;}

	void notifyWindowCreated(ANativeWindow *window);
	void notifyWindowDestroyed(ANativeWindow *window);
	void notifyWindowResized();
	void notifyQueueCreated(AInputQueue *queue);
	void notifyQueueDestroyed(AInputQueue *queue);
	
	void stepEventLoop();
	void windowCreated(ANativeWindow *window);
	void windowDestroyed(ANativeWindow *window);
	void windowResized();
	void queueCreated(AInputQueue *queue);
	void queueDestroyed(AInputQueue *queue);

	void setTitle(const String &title){}
	String getTitle() const{return (char*)NULL;}

	void setPosition(int x,int y){}
	int getPositionX() const{return 0;}
	int getPositionY() const{return 0;}

	void setFullscreen(bool fullscreen){}
	bool getFullscreen() const{return true;}

	void setSize(int width,int height){}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

	void setDifferenceMouse(bool difference){mDifferenceMouse=difference;}
	bool getDifferenceMouse() const{return mDifferenceMouse;}

	void resized(int width,int height)		{if(mApplet!=NULL){mApplet->resized(width,height);}}
	void focusGained()						{if(mApplet!=NULL){mApplet->focusGained();}}
	void focusLost()						{if(mApplet!=NULL){mApplet->focusLost();}}
	void update(int dt)						{if(mApplet!=NULL){mApplet->update(dt);}}
	void render()							{if(mApplet!=NULL){mApplet->render();}}

	void keyPressed(int key)				{if(mApplet!=NULL){mApplet->keyPressed(key);}}
	void keyReleased(int key)				{if(mApplet!=NULL){mApplet->keyReleased(key);}}

	void mousePressed(int x,int y,int button){if(mApplet!=NULL){mApplet->mousePressed(x,y,button);}}
	void mouseMoved(int x,int y)			{if(mApplet!=NULL){mApplet->mouseMoved(x,y);}}
	void mouseReleased(int x,int y,int button){if(mApplet!=NULL){mApplet->mouseReleased(x,y,button);}}
	void mouseScrolled(int x,int y,int scroll){if(mApplet!=NULL){mApplet->mouseScrolled(x,y,scroll);}}

	Engine *getEngine() const{return mEngine;}
	RenderDevice *getRenderDevice() const{return mRenderDevice;}
	AudioDevice *getAudioDevice() const{return mAudioDevice;}
	InputDevice *getInputDevice(InputDevice::InputType type) const{return mInputDevices[type];}

	void setNativeActivity(ANativeActivity *activity);

	void run();
	
protected:
	static void onDestroy(ANativeActivity *activity);
	static void onStart(ANativeActivity *activity);
	static void onStop(ANativeActivity *activity);
	static void onNativeWindowCreated(ANativeActivity *activity,ANativeWindow *window);
	static void onNativeWindowDestroyed(ANativeActivity *activity,ANativeWindow *window);
	static void onNativeWindowResized(ANativeActivity *activity,ANativeWindow *window);
	static void onInputQueueCreated(ANativeActivity *activity,AInputQueue *queue);
	static void onInputQueueDestroyed(ANativeActivity *activity,AInputQueue *queue);
	static void onConfigurationChanged(ANativeActivity* activity);

	ANativeActivity *mActivity;
	AConfiguration *mConfig;
	ALooper *mLooper;
	AInputQueue *mQueue;
	ANativeWindow *mWindow,*mNotifyWindowCreated,*mNotifyWindowDestroyed;
	AInputQueue *mNotifyQueueCreated,*mNotifyQueueDestroyed;
	bool mNotifyWindowResized;
	int mWidth,mHeight;
	int mLastX,mLastY;
	bool mDifferenceMouse;
	Mutex mWindowMutex;
	WaitCondition mWindowCondition;
	Thread::ptr mThread;
	bool mRun;

	WindowRenderTargetFormat::ptr mFormat;
	Engine *mEngine;
	RenderTarget::ptr mRenderTarget;
	RenderDevice::ptr mRenderDevice;
	AudioDevice::ptr mAudioDevice;
	InputDevice::ptr mInputDevices[InputDevice::InputType_MAX];
	Applet *mApplet;
};

}
}

#endif
