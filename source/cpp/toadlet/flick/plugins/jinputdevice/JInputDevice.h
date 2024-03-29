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

class TOADLET_API JInputDevice:public Object,public InputDevice{
public:
	TOADLET_IOBJECT(JInputDevice);

	JInputDevice(JNIEnv *jenv,jobject jobj);
	virtual ~JInputDevice();
	
	bool create();
	void destroy();

	InputType getType();
	bool start();
	void update(int dt);
	void stop();

	void setListener(InputDeviceListener *listener);
	void setSampleTime(int dt);
	void setAlpha(scalar alpha);
	
protected:
	JNIEnv *getEnv() const;

	JavaVM *vm;
	jobject obj;
	jmethodID createID,destroyID,getTypeID,startID,updateID,stopID,setListenerID,setSampleTimeID,setAlphaID;
};

}
}

#endif
