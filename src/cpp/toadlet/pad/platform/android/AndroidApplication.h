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
#include <toadlet/pad/Application.h>
#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>

namespace toadlet{
namespace pad{

class AndroidApplication:public Application,public Runnable{
public:
	AndroidApplication();
	virtual ~AndroidApplication();

	bool create(String renderDevice,String audioDevice,String motionDevice,String joyDevice);
	void destroy();

	void start();
	void stop();
	bool isRunning() const;

	void activate(){}
	void deactivate(){}
	bool isActive() const{return true;}

	void nativeWindowCreated(ANativeWindow *window);
	void nativeWindowDestroyed(ANativeWindow *window);
	
	void setTitle(const String &title){}
	String getTitle() const{return (char*)NULL;}

	void setPosition(int x,int y){}
	int getPositionX() const{return 0;}
	int getPositionY() const{return 0;}

	void setFullscreen(bool fullscreen){}
	bool getFullscreen() const{return true;}

	void setSize(int width,int height){}
	int getWidth() const;
	int getHeight() const;

	void setDifferenceMouse(bool difference);
	bool getDifferenceMouse() const;

	Engine *getEngine() const{return mEngine;}
	RenderDevice *getRenderDevice() const{return mRenderDevice;}
	AudioDevice *getAudioDevice() const{return mAudioDevice;}
	MotionDevice *getMotionDevice() const{return NULL;}
	JoyDevice *getJoyDevice() const{return NULL;}

	void setNativeActivity(ANativeActivity *activity);

	void run();
	
protected:
	static void onDestroy(ANativeActivity *activity);
	static void onStart(ANativeActivity *activity);
	static void onStop(ANativeActivity *activity);
	static void onNativeWindowCreated(ANativeActivity *activity,ANativeWindow *window);
	static void onNativeWindowDestroyed(ANativeActivity *activity,ANativeWindow *window);

	ANativeActivity *mActivity;
	ANativeWindow *mWindow;
	Thread::ptr mThread;
	bool mRunning;

	Engine *mEngine;
	RenderDevice *mRenderDevice;
	AudioDevice *mAudioDevice;
	Applet *mApplet;
};

}
}

#endif
