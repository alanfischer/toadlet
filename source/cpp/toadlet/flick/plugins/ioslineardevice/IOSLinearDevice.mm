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
#include "IOSLinearDevice.h"

@implementation ToadletAccelerometerDelegate

- (id) initWithLinearDevice:(toadlet::flick::IOSLinearDevice*)linearDevice{
	mLinearDevice=linearDevice;
	return self;
}

- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	mLinearDevice->didAccelerate(acceleration);
}

@end

namespace toadlet{
namespace flick{

TOADLET_C_API InputDevice *new_IOSLinearDevice(){
	return new IOSLinearDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API InputDevice* new_LinearDevice(){
		return new IOSLinearDevice();
	}
#endif

IOSLinearDevice::IOSLinearDevice():
	mRunning(false),
	mListener(NULL),
	mAccelerometerDelegate(nil)

{}

bool IOSLinearDevice::create(){
	mAccelerometerDelegate=[[ToadletAccelerometerDelegate alloc] initWithLinearDevice:this];

	return true;
}

bool IOSLinearDevice::start(){
	[UIAccelerometer sharedAccelerometer].updateInterval=0.05;
	[UIAccelerometer sharedAccelerometer].delegate=mAccelerometerDelegate;

	mRunning=true;

	return mRunning;
}

void IOSLinearDevice::stop(){
	[UIAccelerometer sharedAccelerometer].updateInterval=0;
	[UIAccelerometer sharedAccelerometer].delegate=nil;

	mRunning=false;
}

void IOSLinearDevice::destroy(){
	if(mRunning){
		stop();
	}

	if(mAccelerometerDelegate!=nil){
		[mAccelerometerDelegate release];
		mAccelerometerDelegate=nil;
	}
}

void IOSLinearDevice::setSampleTime(int dt){
	[UIAccelerometer sharedAccelerometer].updateInterval=(float)dt/1000.0;
}

void IOSLinearDevice::didAccelerate(UIAcceleration *acceleration){
	mLinearData.type=InputDevice::InputType_LINEAR;
	mLinearData.time=(int)acceleration.timestamp*1000;
	mLinearData.valid=(1<<InputData::Semantic_LINEAR_ACCELERATION);
	mLinearData.values[InputData::Semantic_LINEAR_ACCELERATION].set(Math::fromFloat(acceleration.x),Math::fromFloat(acceleration.y),Math::fromFloat(acceleration.z),0);

	if(mListener!=NULL){
		mListener->inputDetected(mLinearData);
	}
}

}
}
