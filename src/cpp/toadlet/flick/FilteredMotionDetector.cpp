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

#include <toadlet/flick/FilteredMotionDetector.h>
#include <toadlet/egg/math/Math.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace toadlet{
namespace flick{

FilteredMotionDetector::FilteredMotionDetector():
	mAlpha(0.667)
	//mMotionData
{}

FilteredMotionDetector::~FilteredMotionDetector(){
}

void FilteredMotionDetector::setFilterAlpha(real alpha){
	mAlpha=alpha;
}

bool FilteredMotionDetector::updateAcceleration(int time,float x,float y,float z){
	Vector3 &lastAcceleration=cache_updateAcceleration_lastAcceleration.set(mMotionData.acceleration);
	Vector3 &lastVelocity=cache_updateAcceleration_lastAcceleration.set(mMotionData.velocity);
	Vector3 &lastVelocityFiltered=cache_updateAcceleration_lastVelocityFiltered.set(mMotionData.velocityFiltered);
	Vector3 &vector=cache_updateAcceleration_vector;

	mMotionData.acceleration.set(x,y,z);

	if(mMotionData.time==0){
		mMotionData.velocity.reset();
		mMotionData.velocityFiltered.reset();
		mMotionData.time=time;

		return false;
	}
	else{
		// Improved euler integration
		// v = v + 0.5dt * (ao + a)
		Math::add(vector,mMotionData.acceleration,lastAcceleration);
		Math::mul(vector,Math::fromMilli(time-mMotionData.time)/2);
		Math::add(mMotionData.velocity,vector);

		// Highpass filtering
		// vf = alpha*(vf0+v-v0)
		Math::sub(vector,mMotionData.velocity,lastVelocity);
		Math::add(vector,lastVelocityFiltered);
		Math::mul(mMotionData.velocityFiltered,vector,mAlpha);

		mMotionData.time=time;

		return true;		
	}
}

}
}
