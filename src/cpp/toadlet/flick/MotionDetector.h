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

#ifndef TOADLET_FLICK_MOTIONDETECTOR_H
#define TOADLET_FLICK_MOTIONDETECTOR_H

#include <toadlet/flick/Types.h>

namespace toadlet{
namespace flick{

class MotionDetectorListener;

class MotionDetector{
public:
	enum State{
		State_DESTROYED=0,
		State_STOPPED=1,
		State_RUNNING=2,
	};

	class MotionData{
	public:
		MotionData():
			time(0){}

		void set(const MotionData &data){
			time=data.time;
			acceleration.set(data.acceleration);
			velocity.set(data.velocity);
			velocityFiltered.set(data.velocityFiltered);
		}

		uint64 time;
		Vector3 acceleration;
		Vector3 velocity;
		Vector3 velocityFiltered;
	};

	virtual ~MotionDetector(){}

	virtual bool available()=0;

	virtual bool create()=0;

	virtual bool startup()=0;

	virtual bool shutdown()=0;

	virtual bool destroy()=0;

	virtual void setPollSleep(int ms)=0; // Milliseconds to sleep between polls

	virtual void setFilterAlpha(scalar alpha)=0;

	virtual void setNativeOrientation(bool native)=0;

	virtual void setListener(MotionDetectorListener *listener)=0;

	virtual State getState()=0;
};

}
}

#endif
