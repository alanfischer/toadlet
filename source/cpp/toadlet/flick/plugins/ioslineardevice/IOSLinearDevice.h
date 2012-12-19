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

#ifndef TOADLET_FLICK_IOSLINEARDEVICE_H
#define TOADLET_FLICK_IOSLINEARDEVICE_H

#include <toadlet/flick/BaseInputDevice.h>
#include <toadlet/flick/InputData.h>
#include <toadlet/egg/System.h>
#import <UIKit/UIKit.h>

namespace toadlet{
namespace flick{

class IOSLinearDevice;

}
}

@interface ToadletAccelerometerDelegate:NSObject<UIAccelerometerDelegate>{
    toadlet::flick::IOSLinearDevice *mLinearDevice;    
}

- (id) initWithLinearDevice:(toadlet::flick::IOSLinearDevice*)linearDevice;
- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration;

@end

namespace toadlet{
namespace flick{

class IOSLinearDevice:public BaseInputDevice{
public:
	TOADLET_OBJECT(IOSLinearDevice);
	
	IOSLinearDevice();
	virtual ~IOSLinearDevice();

	bool create();
	void destroy();

	InputDevice::InputType getType(){return InputDevice::InputType_LINEAR;}
	bool start();
	void update(int dt){}
	void stop();

	void setListener(InputDeviceListener *listener){mListener=listener;}
	void setSampleTime(int dt);
	void setAlpha(scalar alpha){}

	void didAccelerate(UIAcceleration *acceleration);

protected:
	bool mRunning;
	InputDeviceListener *mListener;
	ToadletAccelerometerDelegate *mAccelerometerDelegate;
	InputData mLinearData;
};

}
}

#endif

