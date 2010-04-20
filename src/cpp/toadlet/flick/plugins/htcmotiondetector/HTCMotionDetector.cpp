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

#include "HTCMotionDetector.h"
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace toadlet{
namespace flick{

TOADLET_C_API MotionDetector *new_HTCMotionDetector(){
	return new HTCMotionDetector();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API MotionDetector* new_MotionDetector(){
		return new HTCMotionDetector();
	}
#endif

HTCMotionDetector::HTCMotionDetector():FilteredMotionDetector(),
	mState(State_DESTROYED),

	mPollSleep(50),
	mNative(false),
	mListener(NULL),

	//mSensorLibrary(),
	pfnHTCSensorOpen(NULL),
	pfnHTCSensorClose(NULL),
	pfnHTCSensorGetDataOutput(NULL),

	mSensor(0),

	//mSensorData,

	//mOuterMutex,
	//mInnerMutex,
	mListenerLocked(false),
	mPollThread(NULL)
{
	if(mSensorLibrary.load(SENSOR_DLL)){
		pfnHTCSensorOpen=(PFN_HTCSensorOpen)mSensorLibrary.getSymbol("HTCSensorOpen");
		pfnHTCSensorClose=(PFN_HTCSensorClose)mSensorLibrary.getSymbol("HTCSensorClose");
		pfnHTCSensorGetDataOutput=(PFN_HTCSensorGetDataOutput)mSensorLibrary.getSymbol("HTCSensorGetDataOutput");

		if(pfnHTCSensorOpen!=NULL && pfnHTCSensorClose!=NULL && pfnHTCSensorGetDataOutput!=NULL){
			mSensor=pfnHTCSensorOpen(SENSOR_TILT);
		}
	}

	Logger::alert(Categories::TOADLET_FLICK,
		String("HTCMotionDetector:")+(mSensor!=NULL));
}

HTCMotionDetector::~HTCMotionDetector(){
	TOADLET_ASSERT(mState==State_DESTROYED);

	if(mSensor!=NULL){
		pfnHTCSensorClose(mSensor);
		mSensor=0;
	}
}

bool HTCMotionDetector::available(){
	return mSensor!=0;
}

bool HTCMotionDetector::create(){
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

bool HTCMotionDetector::startup(){
	if(mState==State_RUNNING){
		return true;
	}

	if(mState!=State_STOPPED){
		Error::sequence(Categories::TOADLET_FLICK,
			"startup called out of sequence");
		return false;
	}

	mState=State_RUNNING;

	HANDLE hEvent=CreateEvent(NULL,TRUE,FALSE,SENSOR_START);
	if(hEvent==0 || GetLastError()!=ERROR_ALREADY_EXISTS){
		Error::unknown(Categories::TOADLET_FLICK,
			"error starting sensor");
		return false;
	}

    SetEvent(hEvent);
	CloseHandle(hEvent);

	mPollThread=new Thread(this);
	mPollThread->start();

	return true;
}

bool HTCMotionDetector::shutdown(){
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

	HANDLE hEvent=CreateEvent(NULL,TRUE,FALSE,SENSOR_STOP);
    if(hEvent==0 || GetLastError()!=ERROR_ALREADY_EXISTS){
		Error::unknown(Categories::TOADLET_FLICK,
			"error stopping sensor");
        return false;
    }

	SetEvent(hEvent);
	CloseHandle(hEvent);

	return true;
}

bool HTCMotionDetector::destroy(){
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

void HTCMotionDetector::setPollSleep(int ms){
	mPollSleep=ms;
}

void HTCMotionDetector::setFilterAlpha(real alpha){
	mAlpha=alpha;
}

void HTCMotionDetector::setNativeOrientation(bool native){
	mNative=native;
}

void HTCMotionDetector::setListener(MotionDetectorListener *listener){
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

MotionDetector::State HTCMotionDetector::getState(){
	return mState;
}

void HTCMotionDetector::run(){
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

		ZeroMemory(&mSensorData,sizeof(mSensorData));
		pfnHTCSensorGetDataOutput(mSensor,&mSensorData);
		// No need to swap anything around, since HTC native orientation is flick orientation
		if(updateAcceleration(System::mtime(),Math::fromMilli(mSensorData.TiltX),Math::fromMilli(mSensorData.TiltY),Math::fromMilli(mSensorData.TiltZ))){
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
