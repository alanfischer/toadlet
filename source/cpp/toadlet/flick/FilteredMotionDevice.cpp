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
	mAlpha(Math::fromMilli(667))
	//mMotionData
{}

FilteredMotionDevice::~FilteredMotionDevice(){
}

void FilteredMotionDevice::setMotionDevice(MotionDevice *device){
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

bool FilteredMotionDevice::isRunning(){
	if(mDevice==NULL){
		return false;
	}
	
	return mDevice->isRunning();
}

void FilteredMotionDevice::setSampleTime(int dt){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->setSampleTime(dt);
}

void FilteredMotionDevice::motionDetected(const MotionData &data){
	Vector3 lastAcceleration=mMotionData.acceleration;
	Vector3 lastRawVelocity=mRawVelocity;
	Vector3 lastVelocity=mMotionData.velocity;
	Vector3 vector;

	mMotionData.acceleration.set(data.acceleration);	

	if(mMotionData.time==0){
		mRawVelocity.reset();
		mMotionData.velocity.reset();
		mMotionData.time=data.time;
	}
	else{
		// Improved euler integration
		// v = v + 0.5dt * (ao + a)
		Math::add(vector,data.acceleration,lastAcceleration);
		Math::mul(vector,Math::fromMilli(data.time-mMotionData.time)/2);
		Math::add(mRawVelocity,vector);

		// Highpass filtering
		// vf = alpha*(vf0+v-v0)
		Math::sub(vector,mRawVelocity,lastRawVelocity);
		Math::add(vector,lastVelocity);
		Math::mul(mMotionData.velocity,vector,mAlpha);

		if(mListener!=NULL){
			mListener->motionDetected(mMotionData);
		}
	}
}

}
}
