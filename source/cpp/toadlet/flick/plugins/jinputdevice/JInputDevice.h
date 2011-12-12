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

#ifndef TOADLET_FLICK_JINPUTDEVICE_H
#define TOADLET_FLICK_JINPUTDEVICE_H

#include <toadlet/flick/InputDevice.h>
#include <jni.h>

namespace toadlet{
namespace flick{

class TOADLET_API JInputDevice:public InputDevice{
public:
	JInputDevice(JNIEnv *jenv,jobject jobj);
	virtual ~JInputDevice();

	virtual bool create();
	virtual void destroy();

	virtual InputType getType();
	virtual bool start();
	virtual void update(int dt);
	virtual void stop();
	virtual bool isRunning();

	virtual void setListener(InputDeviceListener *listener);
	virtual void setSampleTime(int dt);
	virtual void setAlpha(scalar alpha);
	
protected:
	JNIEnv *env;
	jobject obj;
	jmethodID createID,destroyID,getTypeID,startID,updateID,stopID,isRunningID,setListenerID,setSampleTimeID,setAlphaID;
};

}
}

#endif
