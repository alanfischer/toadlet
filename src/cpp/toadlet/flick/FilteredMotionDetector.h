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

#ifndef TOADLET_FLICK_FILTEREDMOTIONDETECTOR_H
#define TOADLET_FLICK_FILTEREDMOTIONDETECTOR_H

#include <toadlet/flick/MotionDetector.h>
#include <toadlet/flick/MotionDetectorListener.h>

namespace toadlet{
namespace flick{

class TOADLET_API FilteredMotionDetector:public MotionDetector{
public:
	FilteredMotionDetector();
	virtual ~FilteredMotionDetector();

	virtual void setFilterAlpha(egg::math::real alpha);

protected:
	virtual bool updateAcceleration(int time,float x,float y,float z);

	egg::math::real mAlpha;
	MotionData mMotionData;

	egg::math::Vector3 cache_updateAcceleration_lastAcceleration;
	egg::math::Vector3 cache_updateAcceleration_lastVelocity;
	egg::math::Vector3 cache_updateAcceleration_lastVelocityFiltered;
	egg::math::Vector3 cache_updateAcceleration_vector;
};

}
}

#endif
