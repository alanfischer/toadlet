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

#ifndef TOADLET_PAD_JANDROIDAPPLICATION_H
#define TOADLET_PAD_JANDROIDAPPLICATION_H

#include <toadlet/pad/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/flick/MotionDevice.h>
#include <jni.h>

namespace toadlet{
namespace pad{

/// @todo: We should have the Application class be an interface, and BaseApplication be the implementation, then JAndroidApplication would just extend Application
class JAndroidApplication{
public:

	enum Key{
		Key_ENTER=10,
		Key_TAB=8,
		Key_SPACE=32,

		Key_LEFT=1024,
		Key_RIGHT,
		Key_UP,
		Key_DOWN,

		// Keyboard keys
		Key_ESC=2048,
		Key_PAUSE,
		Key_SHIFT,
		Key_CTRL,
		Key_ALT,
		Key_SPECIAL,
		Key_BACKSPACE,
		Key_DELETE,

		// Cellphone keys
		Key_SOFTLEFT=4096,
		Key_SOFTRIGHT,
		Key_ACTION,
		Key_BACK,
	};

	JAndroidApplication(JNIEnv *jenv,jobject jobj);
	virtual ~JAndroidApplication();

	virtual void create(String renderDevice=(char*)NULL,String audioDevice=(char*)NULL,String motionDevice=(char*)NULL);
	virtual void destroy();

	virtual void start();
	virtual void stop();

	virtual Engine *getEngine();
	virtual RenderDevice *getRenderDevice();
	virtual MotionDevice *getMotionDevice(){return NULL;}
	
protected:
	JNIEnv *env;
	jobject obj;
	jmethodID createID,destroyID,startID,stopID,getEngineID,getRenderDeviceID,getNativeHandleEngineID,getNativeHandleRenderDeviceID;
};

}
}

#endif
