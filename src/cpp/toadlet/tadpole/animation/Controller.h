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

#ifndef TOADLET_TADPOLE_ANIMATION_CONTROLLER_H
#define TOADLET_TADPOLE_ANIMATION_CONTROLLER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/animation/ControllerFinishedListener.h>

namespace toadlet{
namespace tadpole{
namespace animation{

/// @todo: A nice feature would be the ability for Controllers to be run by a Thread pool, so a central ControllerManager would take
//   care of the updating.  But perhaps this would be better suited by having Nodes update their Animations as we do now, and then the
//   whole scenegraph be updated by a ThreadPool, taking into account dependencies
/// @todo: Controllers should have a general ControllerListener that can be implemented by Node, so when they are destroyed, the node can listen and remove the controller
class TOADLET_API Controller{
public:
	TOADLET_SHARED_POINTERS(Controller);

	enum Interpolation{
		Interpolation_LINEAR,
		Interpolation_COS,
	};

	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	Controller();
	virtual ~Controller();

	virtual void setTime(int time,bool setagain=true);
	inline int getTime() const{return mTime;}

	virtual void setCycling(Cycling cycling);
	inline Cycling getCycling() const{return mCycling;}

	virtual void setInterpolation(Interpolation interpolation);
	inline Interpolation getInterpolation() const{return mInterpolation;}

	virtual void setTimeScale(scalar scale);
	inline scalar getTimeScale() const{return mTimeScale;}

	virtual void setControllerFinishedListener(ControllerFinishedListener *listener,bool owns);
	inline ControllerFinishedListener *getControllerFinishedListener() const{return mFinishedListener;}

	virtual void setMinMaxValue(scalar minValue,scalar maxValue){mMinValue=minValue;mMaxValue=maxValue;}
	inline scalar getMinValue() const{return mMinValue;}
	inline scalar getMaxValue() const{return mMaxValue;}

	virtual void start();
	virtual void stop();
	inline bool isRunning() const{return mRunning;}

	virtual void update(int dt);

	virtual void set(scalar value);

	virtual void extentsChanged();

	virtual void attach(Animatable::ptr animatable);
	virtual void remove(Animatable::ptr animatable);

protected:
	Collection<Animatable::ptr> mAnimatables;

	int mTime;
	scalar mMinValue;
	scalar mMaxValue;
	int mMinTime;
	int mMaxTime;
	Cycling mCycling;
	Interpolation mInterpolation;
	scalar mTimeScale;
	bool mRunning;
	ControllerFinishedListener *mFinishedListener;
	bool mOwnsFinishedListener;
};

}
}
}

#endif
