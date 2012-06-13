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

#ifndef TOADLET_TADPOLE_ANIMATIONACTIONCOMPONENT_H
#define TOADLET_TADPOLE_ANIMATIONACTIONCOMPONENT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/BaseActionComponent.h>
#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/animation/Interpolator.h>
#include <toadlet/tadpole/animation/CosInterpolator.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API AnimationActionComponent:public BaseActionComponent,public AnimationListener{
public:
	TOADLET_IPTR(AnimationActionComponent);

	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	AnimationActionComponent(const String &name);
	virtual ~AnimationActionComponent();

	virtual void frameUpdate(int dt,int scope);

	virtual bool getActive() const{return mRunning;}

	virtual void setTime(int time);
	inline int getTime() const{return mTime;}

	virtual void setTimeScale(scalar scale);
	inline scalar getTimeScale() const{return mTimeScale;}

	virtual void setCycling(Cycling cycling);
	inline Cycling getCycling() const{return mCycling;}

	virtual void setStopGently(bool stop){mStopGently=stop;}
	inline bool getStopGently() const{return mStopGently;}

	virtual void setInterpolator(Interpolator *interpolator);
	inline Interpolator *getInterpolator() const{return mInterpolator;}

	virtual void setMinMaxValue(scalar minValue,scalar maxValue){mMinValue=minValue;mMaxValue=maxValue;}
	inline scalar getMinValue() const{return mMinValue;}
	inline scalar getMaxValue() const{return mMaxValue;}

	virtual void start();
	virtual void stop();

	virtual void setValue(scalar value);

	virtual void attach(Animation *animation);
	virtual void remove(Animation *animation);

	virtual void animationExtentsChanged(Animation *animation);

protected:
	Collection<Animation::ptr> mAnimations;

	Node *mNode;
	int mTime;
	scalar mMinValue;
	scalar mMaxValue;
	int mMinTime;
	int mMaxTime;
	Cycling mCycling;
	Cycling mActualCycling;
	bool mResetOnStart;
	bool mStopGently;
	Interpolator::ptr mInterpolator;
	scalar mTimeScale;
	bool mRunning;
};

}
}

#endif
