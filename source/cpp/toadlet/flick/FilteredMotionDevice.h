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

#ifndef TOADLET_FLICK_FILTEREDMOTIONDEVICE_H
#define TOADLET_FLICK_FILTEREDMOTIONDEVICE_H

#include <toadlet/flick/InputDevice.h>
#include <toadlet/flick/InputDeviceListener.h>

namespace toadlet{
namespace flick{

class TOADLET_API FilteredMotionDevice:public InputDevice,public InputDeviceListener{
public:
	FilteredMotionDevice();
	virtual ~FilteredMotionDevice();

	virtual void setMotionDevice(InputDevice *device);
	
	virtual bool create();
	virtual void destroy();

	virtual InputType getType();
	virtual bool start();
	virtual void update(int dt);
	virtual void stop();

	virtual void setListener(InputDeviceListener *listener){mListener=listener;}
	virtual void setSampleTime(int dt);
	virtual void setAlpha(scalar alpha){mAlpha=alpha;}

	virtual void inputDetected(const InputData &data);
	
protected:
	InputDeviceListener *mListener;
	InputDevice *mDevice;
	scalar mAlpha;
	InputData mMotionData;
	Vector4 mRawVelocity;
};

}
}

#endif
