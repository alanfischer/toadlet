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
#include "IPhoneMotionDetector.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;

@implementation ToadletAccelerometerDelegate

- (id) initWithMotionDetector:(toadlet::flick::IPhoneMotionDetector*)motionDetector{
	mMotionDetector=motionDetector;
	return self;
}

- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	mMotionDetector->didAccelerate(acceleration);
}

@end

namespace toadlet{
namespace flick{

bool IPhoneMotionDetector::available(){
	return true;
}

TOADLET_C_API MotionDetector *new_IPhoneMotionDetector(){
	return new IPhoneMotionDetector();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API MotionDetector* new_MotionDetector(){
		return new IPhoneMotionDetector();
	}
#endif

IPhoneMotionDetector::IPhoneMotionDetector():
	mState(State_DESTROYED),
	mListener(NULL),
	mNative(false),
	mAccelerometerDelegate(nil)

{}

IPhoneMotionDetector::~IPhoneMotionDetector(){
	TOADLET_ASSERT(mState==State_DESTROYED);
}

bool IPhoneMotionDetector::create(){
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

bool IPhoneMotionDetector::startup(){
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

bool IPhoneMotionDetector::shutdown(){
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

bool IPhoneMotionDetector::destroy(){
	if(mState==State_DESTROYED){
		return true;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"destroy called out of sequence");
		return false;
	}

	mState=State_DESTROYED;

	[mAccelerometerDelegate release];
	
	return true;
}

void IPhoneMotionDetector::setPollSleep(int ms){
	[UIAccelerometer sharedAccelerometer].updateInterval=(float)ms/1000.0;
}

void IPhoneMotionDetector::setNativeOrientation(bool native){
	mNative=native;
}

void IPhoneMotionDetector::setListener(MotionDetectorListener *listener){
	mMotionData.time=0;
	mListener=listener;
}

MotionDetector::State IPhoneMotionDetector::getState(){
	return mState;
}

void IPhoneMotionDetector::didAccelerate(UIAcceleration *acceleration){
	bool result=false;
	if(mNative){
		result=updateAcceleration((int)acceleration.timestamp*1000,acceleration.x,acceleration.y,acceleration.z);
	}
	else{
		result=updateAcceleration((int)acceleration.timestamp*1000,acceleration.x,-acceleration.y,acceleration.z);
	}
	if(result && mListener!=NULL){
		mListener->motionDetected(mMotionData);
	}
}

}
}
