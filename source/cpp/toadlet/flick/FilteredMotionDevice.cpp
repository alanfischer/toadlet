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

#include <toadlet/flick/FilteredMotionDevice.h>

namespace toadlet{
namespace flick{

FilteredMotionDevice::FilteredMotionDevice():
	mListener(NULL),
	mDevice(NULL),
	mAlpha(Math::fromMilli(667)),
	mMotionData(InputType_MOTION,0,InputData::Semantic_MAX_MOTION)
{
}

FilteredMotionDevice::~FilteredMotionDevice(){
}

void FilteredMotionDevice::setMotionDevice(InputDevice *device){
	mDevice=device;
}

bool FilteredMotionDevice::create(){
	if(mDevice==NULL){
		return false;
	}

	return mDevice->create();
}

void FilteredMotionDevice::destroy(){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->destroy();
}

InputDevice::InputType FilteredMotionDevice::getType(){
	if(mDevice==NULL){
		return (InputType)0;
	}
	
	return mDevice->getType();
}

bool FilteredMotionDevice::start(){
	if(mDevice==NULL){
		return false;
	}
	
	return mDevice->start();
}

void FilteredMotionDevice::update(int dt){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->update(dt);
}

void FilteredMotionDevice::stop(){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->stop();
}

void FilteredMotionDevice::setSampleTime(int dt){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->setSampleTime(dt);
}

void FilteredMotionDevice::inputDetected(const InputData &data){
	Vector4 lastAcceleration=mMotionData.values[InputData::Semantic_MOTION_ACCELERATION];
	Vector4 lastVelocity=mMotionData.values[InputData::Semantic_MOTION_VELOCITY];
	Vector4 lastRawVelocity=mRawVelocity;
	Vector4 newAcceleration=data.values[InputData::Semantic_MOTION_ACCELERATION];
	Vector4 newVelocity=data.values[InputData::Semantic_MOTION_VELOCITY];
	Vector4 vector;

	mMotionData.valid=data.valid;
	mMotionData.values[InputData::Semantic_MOTION_ACCELERATION].set(newAcceleration);

	if(mMotionData.time==0){
		mMotionData.time=data.time;
		mMotionData.values[InputData::Semantic_MOTION_VELOCITY].reset();
		mRawVelocity.reset();
	}
	else{
		mMotionData.time=data.time;

		// Improved euler integration
		// v = v + 0.5dt * (ao + a)
		Math::add(vector,newAcceleration,lastAcceleration);
		Math::mul(vector,Math::fromMilli(data.time-mMotionData.time)/2);
		Math::add(mRawVelocity,vector);

		// Highpass filtering
		// vf = alpha*(vf0+v-v0)
		Math::sub(vector,mRawVelocity,lastRawVelocity);
		Math::add(vector,lastVelocity);
		Math::mul(newVelocity,vector,mAlpha);

		mMotionData.values[InputData::Semantic_MOTION_VELOCITY].set(newVelocity);
		
		if(mListener!=NULL){
			mListener->inputDetected(mMotionData);
		}
	}
}

}
}
