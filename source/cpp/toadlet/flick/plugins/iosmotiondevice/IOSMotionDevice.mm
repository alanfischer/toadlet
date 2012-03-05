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
#include <toadlet/flick/InputDeviceListener.h>
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

TOADLET_C_API InputDevice *new_IOSMotionDevice(){
	return new IOSMotionDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API InputDevice* new_MotionDevice(){
		return new IOSMotionDevice();
	}
#endif

IOSMotionDevice::IOSMotionDevice():Object(),
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

void IOSMotionDevice::stop(){
	[UIAccelerometer sharedAccelerometer].updateInterval=0;
	[UIAccelerometer sharedAccelerometer].delegate=nil;

	mRunning=false;
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

void IOSMotionDevice::didAccelerate(UIAcceleration *acceleration){
	mMotionData.type=InputDevice::InputType_MOTION;
	mMotionData.time=(int)acceleration.timestamp*1000;
	mMotionData.valid=(1<<InputData::Semantic_MOTION_ACCELERATION);
	mMotionData.values[InputData::Semantic_MOTION_ACCELERATION].set(Math::fromFloat(acceleration.x),Math::fromFloat(acceleration.y),Math::fromFloat(acceleration.z),0);

	if(mListener!=NULL){
		mListener->inputDetected(mMotionData);
	}
}

}
}
