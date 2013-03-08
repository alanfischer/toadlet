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

#include <toadlet/flick/FilteredLinearDevice.h>

namespace toadlet{
namespace flick{

FilteredLinearDevice::FilteredLinearDevice():
	mListener(NULL),
	mDevice(NULL),
	mAlpha(Math::fromMilli(667)),
	mLinearData(InputType_LINEAR,0,InputData::Semantic_MAX_LINEAR)
{
}

FilteredLinearDevice::~FilteredLinearDevice(){
}

void FilteredLinearDevice::setLinearDevice(InputDevice *device){
	mDevice=device;
}

bool FilteredLinearDevice::create(){
	if(mDevice==NULL){
		return false;
	}

	return mDevice->create();
}

void FilteredLinearDevice::destroy(){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->destroy();
}

InputDevice::InputType FilteredLinearDevice::getType(){
	if(mDevice==NULL){
		return (InputType)0;
	}
	
	return mDevice->getType();
}

bool FilteredLinearDevice::start(){
	if(mDevice==NULL){
		return false;
	}
	
	return mDevice->start();
}

void FilteredLinearDevice::update(int dt){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->update(dt);
}

void FilteredLinearDevice::stop(){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->stop();
}

void FilteredLinearDevice::setSampleTime(int dt){
	if(mDevice==NULL){
		return;
	}
	
	mDevice->setSampleTime(dt);
}

void FilteredLinearDevice::inputDetected(const InputData &data){
	Vector4 lastAcceleration=mLinearData.values[InputData::Semantic_LINEAR_ACCELERATION];
	Vector4 lastVelocity=mLinearData.values[InputData::Semantic_LINEAR_VELOCITY];
	Vector4 lastRawVelocity=mRawVelocity;
	Vector4 newAcceleration=data.values[InputData::Semantic_LINEAR_ACCELERATION];
	Vector4 newVelocity=data.values[InputData::Semantic_LINEAR_VELOCITY];
	Vector4 vector;

	mLinearData.valid=data.valid;
	mLinearData.values[InputData::Semantic_LINEAR_ACCELERATION].set(newAcceleration);

	if(mLinearData.time==0){
		mLinearData.time=data.time;
		mLinearData.values[InputData::Semantic_LINEAR_VELOCITY].reset();
		mRawVelocity.reset();
	}
	else{
		// Improved euler integration
		// v = v + 0.5dt * (ao + a)
		Math::add(vector,newAcceleration,lastAcceleration);
		Math::mul(vector,Math::fromMilli(data.time-mLinearData.time)/2);
		Math::add(mRawVelocity,vector);

		// Highpass filtering
		// vf = alpha*(vf0+v-v0)
		Math::sub(vector,mRawVelocity,lastRawVelocity);
		Math::add(vector,lastVelocity);
		Math::mul(newVelocity,vector,mAlpha);

		mLinearData.time=data.time;
		mLinearData.values[InputData::Semantic_LINEAR_VELOCITY].set(newVelocity);
		
		if(mListener!=NULL){
			mListener->inputDetected(mLinearData);
		}
	}
}

}
}
