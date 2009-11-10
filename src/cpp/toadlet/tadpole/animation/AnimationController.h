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

#ifndef TOADLET_TADPOLE_ANIMATION_ANIMATIONCONTROLLER_H
#define TOADLET_TADPOLE_ANIMATION_ANIMATIONCONTROLLER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/animation/AnimationControllerFinishedListener.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class TOADLET_API AnimationController{
public:
	TOADLET_SHARED_POINTERS(AnimationController);

	enum Interpolation{
		Interpolation_LINEAR,
		Interpolation_COS,
	};

	AnimationController();
	virtual ~AnimationController(){}

	virtual void setTime(int time,bool setagain=true);
	inline int getTime() const{return mLogicTime;}

	virtual void setLoop(bool loop);
	inline bool getLoop() const{return mLoop;}

	virtual void setInterpolation(Interpolation interpolation);
	inline Interpolation getInterpolation() const{return mInterpolation;}

	virtual void setTimeScale(scalar scale);
	inline scalar getTimeScale() const{return mTimeScale;}

	virtual void setAnimationControllerFinishedListener(AnimationControllerFinishedListener *listener);
	inline AnimationControllerFinishedListener *getAnimationControllerFinishedListener() const{return mFinishedListener;}

	virtual void setMinMaxValue(scalar minValue,scalar maxValue){mMinValue=minValue;mMaxValue=maxValue;}
	inline scalar getMinValue() const{return mMinValue;}
	inline scalar getMaxValue() const{return mMaxValue;}

	virtual void start();
	virtual void stop();
	inline bool isRunning() const{return mRunning;}

	virtual void logicUpdate(int dt);
	virtual void visualUpdate(int dt);

	virtual void set(scalar value);

	virtual void extentsChanged();

	virtual void attachAnimation(Animation::ptr animation);
	virtual void removeAnimation(Animation::ptr animation);

protected:
	egg::Collection<Animation::ptr> mAnimations;

	int mLogicTime;
	int mVisualTime;
	scalar mMinValue;
	scalar mMaxValue;
	int mMinTime;
	int mMaxTime;
	bool mLoop;
	Interpolation mInterpolation;
	scalar mTimeScale;
	bool mRunning;
	AnimationControllerFinishedListener *mFinishedListener;
};

}
}
}

#endif
