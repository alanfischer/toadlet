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

#ifndef TOADLET_FLICK_ANDROIDSENSORDEVICE_H
#define TOADLET_FLICK_ANDROIDSENSORDEVICE_H

#include <toadlet/flick/InputDevice.h>
#include <toadlet/flick/InputData.h>
#include <android/sensor.h>

namespace toadlet{
namespace flick{

class TOADLET_API AndroidSensorDevice:protected Object,public InputDevice{
public:
	TOADLET_OBJECT(AndroidSensorDevice);

	AndroidSensorDevice(int type);
	virtual ~AndroidSensorDevice(){}

	virtual bool create();
	virtual void destroy();

	virtual InputType getType(){return mInputType;}
	virtual bool start();
	virtual void update(int dt){}
	virtual void stop();

	virtual void setListener(InputDeviceListener *listener){mListener=listener;}
	virtual void setSampleTime(int dt);
	virtual void setAlpha(scalar alpha){}
	
protected:
	void onSensorChanged();

	static InputType getInputTypeFromSensorType(int sensorType);
	static int sensorChanged(int fd,int events,void  *data){((AndroidSensorDevice*)data)->onSensorChanged();return 1;}

	ASensorManager *mSensorManager;
	ASensorEventQueue *mEventQueue;
	int mSensorType;
	const ASensor *mSensor;
	InputType mInputType;
	bool mRunning;
	InputDeviceListener *mListener;
	InputData mData;
};

}
}

#endif
