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

#include "AndroidSensorDevice.h"
#include <toadlet/flick/InputDeviceListener.h>

namespace toadlet{
namespace flick{

AndroidSensorDevice::AndroidSensorDevice(int type):Object(),
	mSensorManager(NULL),
	mEventQueue(NULL),
	mSensorType(0),
	mSensor(NULL),
	mInputType((InputType)0),
	mRunning(false),
	mListener(NULL),
	mData(getInputTypeFromSensorType(type),0,2)
{
	mSensorManager=ASensorManager_getInstance();
	mSensorType=type;
	mInputType=getInputTypeFromSensorType(mSensorType);
}

bool AndroidSensorDevice::create(){
	ALooper *looper=ALooper_forThread();
	if(looper==NULL){
		looper=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
	}
	
	mEventQueue=ASensorManager_createEventQueue(mSensorManager,looper,0,sensorChanged,this);

	mSensor=ASensorManager_getDefaultSensor(mSensorManager,mSensorType);
	return mSensor!=NULL;
}

void AndroidSensorDevice::destroy(){	
	stop();

	mSensor=NULL;
	
	if(mEventQueue!=NULL){
		ASensorManager_destroyEventQueue(mSensorManager,mEventQueue);
		mEventQueue=NULL;
	}
}

bool AndroidSensorDevice::start(){
	mRunning=false;
	if(mSensor!=NULL){
		mRunning=ASensorEventQueue_enableSensor(mEventQueue,mSensor)>0;
	}
	return mRunning;
}
	
void AndroidSensorDevice::stop(){
	if(mSensor!=NULL){
		ASensorEventQueue_disableSensor(mEventQueue,mSensor);
	}
	mRunning=false;
}

void AndroidSensorDevice::setSampleTime(int dt){
	if(mSensor!=NULL){
		ASensorEventQueue_setEventRate(mEventQueue,mSensor,dt*1000);
	}
}

void AndroidSensorDevice::onSensorChanged(){
	ASensorEvent event;
	int numEvents=0;
	while((numEvents=ASensorEventQueue_getEvents(mEventQueue,&event,1))>0){
		mData.time=event.timestamp/1000;
		
		switch(mInputType){
			case InputType_MOTION:
				mData.valid=(1<<InputData::Semantic_MOTION_ACCELERATION);
				memcpy(&mData.values[InputData::Semantic_MOTION_ACCELERATION],event.data,sizeof(Vector4));
			break;
			case InputType_ANGULAR:
				mData.valid=(1<<InputData::Semantic_ANGULAR);
				memcpy(&mData.values[InputData::Semantic_ANGULAR],event.data,sizeof(Vector4));
			break;
			case InputType_LIGHT:
				mData.valid=(1<<InputData::Semantic_LIGHT);
				memcpy(&mData.values[InputData::Semantic_LIGHT],event.data,sizeof(Vector4));
			break;
			case InputType_PROXIMITY:
				mData.valid=(1<<InputData::Semantic_PROXIMITY);
				memcpy(&mData.values[InputData::Semantic_PROXIMITY],event.data,sizeof(Vector4));
			break;
			case InputType_MAGNETIC:
				mData.valid=(1<<InputData::Semantic_MAGNETIC);
				memcpy(&mData.values[InputData::Semantic_MAGNETIC],event.data,sizeof(Vector4));
			break;
			default:
				memcpy(&mData.values[0],event.data,sizeof(Vector4));
			break;
		}

		if(mListener!=NULL){
			mListener->inputDetected(mData);
		}
	}
}

InputDevice::InputType AndroidSensorDevice::getInputTypeFromSensorType(int sensorType){
	switch(sensorType){
		case ASENSOR_TYPE_ACCELEROMETER:
			return InputType_MOTION;
		case ASENSOR_TYPE_GYROSCOPE:
			return InputType_ANGULAR;
		case ASENSOR_TYPE_LIGHT:
			return InputType_LIGHT;
		case ASENSOR_TYPE_PROXIMITY:
			return InputType_PROXIMITY;
		case ASENSOR_TYPE_MAGNETIC_FIELD:
			return InputType_MAGNETIC;
		default:
			return InputType_MAX;
	}
}

}
}
