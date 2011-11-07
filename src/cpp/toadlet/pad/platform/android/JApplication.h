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

#ifndef TOADLET_PAD_JAPPLICATION_H
#define TOADLET_PAD_JAPPLICATION_H

#include <toadlet/pad/Application.h>
#include <jni.h>

namespace toadlet{
namespace pad{

class JApplication:public Application{
public:
	JApplication(JNIEnv *jenv,jobject jobj);
	virtual ~JApplication();

	bool create(String renderDevice,String audioDevice,String motionDevice,String joyDevice);
	void destroy();

	void start();
	void stop();
	bool isRunning() const;

	void activate(){}
	void deactivate(){}
	bool isActive() const{return true;}

	void setTitle(const String &title);
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

	// Integrate a JApplet class here
	void setApplet(Applet *applet){}
	Applet *getApplet() const{return NULL;}

	Engine *getEngine() const;
	RenderDevice *getRenderDevice() const;
	AudioDevice *getAudioDevice() const;
	MotionDevice *getMotionDevice() const{return NULL;}
	JoyDevice *getJoyDevice() const{return NULL;}

protected:
	JNIEnv *env;
	jobject obj;
	jmethodID createID,destroyID,startID,stopID,isRunningID,setDifferenceMouseID,getDifferenceMouseID,getWidthID,getHeightID,getEngineID,getRenderDeviceID,getAudioDeviceID,setNativeHandleID,getNativeHandleID;

	mutable Engine *mEngine;
	mutable jobject mLastEngineObj;
	mutable RenderDevice *mRenderDevice;
	mutable jobject mLastRenderDeviceObj;
	mutable AudioDevice *mAudioDevice;
	mutable jobject mLastAudioDeviceObj;
};

}
}

#endif
