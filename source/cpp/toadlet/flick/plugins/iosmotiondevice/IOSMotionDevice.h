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

#ifndef TOADLET_FLICK_IOSMOTIONDEVICE_H
#define TOADLET_FLICK_IOSMOTIONDEVICE_H

#include <toadlet/flick/FilteredMotionDevice.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#import <UIKit/UIKit.h>

namespace toadlet{
namespace flick{

class IOSMotionDevice;

}
}

@interface ToadletAccelerometerDelegate:NSObject<UIAccelerometerDelegate>;

- (id) initWithMotionDevice:(toadlet::flick::IOSMotionDevice*)motionDevice;
- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration;
toadlet::flick::IOSMotionDevice *mMotionDevice;

@end

namespace toadlet{
namespace flick{

class IOSMotionDevice:public MotionDevice{
public:
	IOSMotionDevice();
	virtual ~IOSMotionDevice();

	bool create();
	void destroy();

	bool start();
	void update(int dt){}
	bool stop();
	bool isRunning(){return mRunning;}

	void setSampleTime(int dt);
	void setListener(MotionDeviceListener *listener);

	void didAccelerate(UIAcceleration *acceleration);

protected:
	bool mRunning;
	MotionDeviceListener *mListener;
	ToadletAccelerometerDelegate *mAccelerometerDelegate;
};

}
}

#endif

