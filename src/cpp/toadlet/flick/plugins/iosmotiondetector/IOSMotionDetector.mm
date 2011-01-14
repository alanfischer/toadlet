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

#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Error.h>
#include "IOSMotionDetector.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;

@implementation ToadletAccelerometerDelegate

- (id) initWithMotionDetector:(toadlet::flick::IOSMotionDetector*)motionDetector{
	mMotionDetector=motionDetector;
	return self;
}

- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	mMotionDetector->didAccelerate(acceleration);
}

@end

namespace toadlet{
namespace flick{

bool IOSMotionDetector::available(){
	return true;
}

TOADLET_C_API MotionDetector *new_IOSMotionDetector(){
	return new IOSMotionDetector();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API MotionDetector* new_MotionDetector(){
		return new IOSMotionDetector();
	}
#endif

IOSMotionDetector::IOSMotionDetector():
	mState(State_DESTROYED),
	mListener(NULL),
	mNative(false),
	mAccelerometerDelegate(nil)

{}

IOSMotionDetector::~IOSMotionDetector(){
	TOADLET_ASSERT(mState==State_DESTROYED);
}

bool IOSMotionDetector::create(){
	if(mState==State_STOPPED){
		return true;
	}

	if(mState!=State_DESTROYED){
		Error::sequence(Categories::TOADLET_FLICK,
			"create called out of sequence");
		return false;
	}

	mState=State_STOPPED;

	mAccelerometerDelegate=[[ToadletAccelerometerDelegate alloc] initWithMotionDetector:this];

	return true;
}

bool IOSMotionDetector::startup(){
	if(mState==State_RUNNING){
		return true;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"startup called out of sequence");
		return false;
	}

	mState=State_RUNNING;

	[UIAccelerometer sharedAccelerometer].updateInterval=0.05;
	[UIAccelerometer sharedAccelerometer].delegate=mAccelerometerDelegate;

	return true;
}

bool IOSMotionDetector::shutdown(){
	if(mState==State_STOPPED){
		return true;
	}

	if(mState!=State_RUNNING){
		Error::sequence(Categories::TOADLET_FLICK,
			"shutdown called out of sequence");
		return false;
	}

	mState=State_STOPPED;

	[UIAccelerometer sharedAccelerometer].delegate=nil;

	return true;
}

void IOSMotionDetector::destroy(){
	if(mState==State_DESTROYED){
		return;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"destroy called out of sequence");
		return;
	}

	mState=State_DESTROYED;

	[mAccelerometerDelegate release];
}

void IOSMotionDetector::setPollSleep(int ms){
	[UIAccelerometer sharedAccelerometer].updateInterval=(float)ms/1000.0;
}

void IOSMotionDetector::setNativeOrientation(bool native){
	mNative=native;
}

void IOSMotionDetector::setListener(MotionDetectorListener *listener){
	mMotionData.time=0;
	mListener=listener;
}

MotionDetector::State IOSMotionDetector::getState(){
	return mState;
}

void IOSMotionDetector::didAccelerate(UIAcceleration *acceleration){
	bool result=false;
	if(mNative){
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
