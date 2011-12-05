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

#include <toadlet/egg/Error.h>
#include "IOSMotionDevice.h"

@implementation ToadletAccelerometerDelegate

- (id) initWithMotionDevice:(toadlet::flick::IOSMotionDevice*)motionDevice{
	mMotionDevice=motionDevice;
	return self;
}

- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	mMotionDevice->didAccelerate(acceleration);
}

@end

namespace toadlet{
namespace flick{

TOADLET_C_API MotionDevice *new_IOSMotionDevice(){
	return new IOSMotionDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API MotionDevice* new_MotionDevice(){
		return new IOSMotionDevice();
	}
#endif

IOSMotionDevice::IOSMotionDevice():
	mRunning(false),
	mListener(NULL),
	mAccelerometerDelegate(nil)

{}

IOSMotionDevice::~IOSMotionDevice(){
	destroy();
}

bool IOSMotionDevice::create(){
	mAccelerometerDelegate=[[ToadletAccelerometerDelegate alloc] initWithMotionDevice:this];

	return true;
}

bool IOSMotionDevice::start(){
	[UIAccelerometer sharedAccelerometer].updateInterval=0.05;
	[UIAccelerometer sharedAccelerometer].delegate=mAccelerometerDelegate;

	mRunning=true;

	return mRunning;
}

bool IOSMotionDevice::stop(){
	[UIAccelerometer sharedAccelerometer].updateInterval=0;
	[UIAccelerometer sharedAccelerometer].delegate=nil;

	mRunning=false;

	return mRunning;
}

void IOSMotionDevice::destroy(){
	if(mRunning){
		stop();
	}

	if(mAccelerometerDelegate!=nil){
		[mAccelerometerDelegate release];
		mAccelerometerDelegate=nil;
	}
}

void IOSMotionDevice::setSampleTime(int dt){
	[UIAccelerometer sharedAccelerometer].updateInterval=(float)dt/1000.0;
}

void IOSMotionDevice::setListener(MotionDeviceListener *listener){
	mMotionData.time=0;
	mListener=listener;
}

void IOSMotionDevice::didAccelerate(UIAcceleration *acceleration){
	bool result=false;
	
	result=updateAcceleration((int)acceleration.timestamp*1000,Math::fromFloat(acceleration.x),Math::fromFloat(acceleration.y),Math::fromFloat(acceleration.z));
	}
	else{
		result=updateAcceleration((int)acceleration.timestamp*1000,Math::fromFloat(acceleration.x),Math::fromFloat(-acceleration.y),Math::fromFloat(acceleration.z));
	} 
	if(result && mListener!=NULL){
		mListener->motionDetected(mMotionData);
	}
}

}
}
