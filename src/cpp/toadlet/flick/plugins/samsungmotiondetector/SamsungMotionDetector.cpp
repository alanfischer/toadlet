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

#include "SamsungMotionDetector.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace toadlet{
namespace flick{

TOADLET_C_API MotionDetector *new_SamsungMotionDetector(){
	return new SamsungMotionDetector();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API MotionDetector* new_MotionDetector(){
		return new SamsungMotionDetector();
	}
#endif

SamsungMotionDetector::SamsungMotionDetector():
	mState(State_DESTROYED),

	mPollSleep(50),
	mNative(false),
	mListener(NULL),

	mSensor(INVALID_HANDLE_VALUE),

	//mOuterMutex,
	//mInnerMutex,
	mListenerLocked(false),
	mPollThread(NULL)
{
	mSensor=CreateFile(TILT_SENSOR,0,0,NULL,OPEN_EXISTING,0,NULL);

	Logger::alert(Categories::TOADLET_FLICK,
		String("SamsungMotionDetector:")+(mSensor!=INVALID_HANDLE_VALUE));
}

SamsungMotionDetector::~SamsungMotionDetector(){
	TOADLET_ASSERT(mState==State_DESTROYED);

	if(mSensor!=INVALID_HANDLE_VALUE){
		CloseHandle(mSensor);
		mSensor=INVALID_HANDLE_VALUE;
	}
}

bool SamsungMotionDetector::available(){
	return mSensor!=INVALID_HANDLE_VALUE;
}

bool SamsungMotionDetector::create(){
	if(mState==State_STOPPED){
		return true;
	}

	if(mState!=State_DESTROYED){
		Error::sequence(Categories::TOADLET_FLICK,
			"create called out of sequence");
		return false;
	}

	mState=State_STOPPED;

	return true;
}

bool SamsungMotionDetector::startup(){
	if(mState==State_RUNNING){
		return true;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"startup called out of sequence");
		return false;
	}

	mState=State_RUNNING;

	DeviceIoControl(mSensor,CONTROL_TILT_ON,NULL,0,NULL,0,NULL,NULL);

	mPollThread=new Thread(this);
	mPollThread->start();

	return true;
}

bool SamsungMotionDetector::shutdown(){
	if(mState==State_STOPPED){
		return true;
	}

	if(mState!=State_RUNNING){
		Error::sequence(Categories::TOADLET_FLICK,
			"shutdown called out of sequence");
		return false;
	}

	mState=State_STOPPED;

	while(mPollThread!=NULL){
		System::msleep(50);
	}

	DeviceIoControl(mSensor,CONTROL_TILT_OFF,NULL,0,NULL,0,NULL,NULL);

	return true;
}

bool SamsungMotionDetector::destroy(){
	if(mState==State_DESTROYED){
		return true;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"destroy called out of sequence");
		return false;
	}

	mState=State_DESTROYED;

	return true;
}

void SamsungMotionDetector::setPollSleep(int ms){
	mPollSleep=ms;
}

void SamsungMotionDetector::setNativeOrientation(bool native){
	mNative=native;
}

void SamsungMotionDetector::setListener(MotionDetectorListener *listener){
	mOuterMutex.lock();
		mMotionData.time=0;
		if(mListenerLocked==false){
			mInnerMutex.lock();
				mListener=listener;
			mInnerMutex.unlock();
		}
		else{
			mListener=listener;
		}
	mOuterMutex.unlock();
}

MotionDetector::State SamsungMotionDetector::getState(){
	return mState;
}

void SamsungMotionDetector::run(){
	while(mState==State_RUNNING){
		System::msleep(mPollSleep);

		bool skip=false;
		mOuterMutex.lock();
			mListenerLocked=true;
		mInnerMutex.lock();
		mOuterMutex.unlock();
			if(mListener==NULL){
				skip=true;
			}
		mOuterMutex.lock();
		mInnerMutex.unlock();
			mListenerLocked=false;
		mOuterMutex.unlock();

		if(skip){
			continue;
		}

		int values[3]={0};
		real scale=9.8 * 3.3793103448275862068965517241379;
		DeviceIoControl(mSensor,CONTROL_TILT_READ,NULL,0,values,sizeof(values),NULL,NULL);
		bool result=false;
		if(mNative){
			result=updateAcceleration(System::mtime(),values[0]*scale,values[1]*scale,values[2]*scale);
		}
		else{
			result=updateAcceleration(System::mtime(),values[1]*scale,values[0]*scale,-values[2]*scale);
		}
		if(result){
			mOuterMutex.lock();
				mListenerLocked=true;
			mInnerMutex.lock();
			mOuterMutex.unlock();
				if(mListener!=NULL){
					mListener->motionDetected(mMotionData);
				}
			mOuterMutex.lock();
			mInnerMutex.unlock();
				mListenerLocked=false;
			mOuterMutex.unlock();
		}
	}

	delete mPollThread;
	mPollThread=NULL;
}

}
}
