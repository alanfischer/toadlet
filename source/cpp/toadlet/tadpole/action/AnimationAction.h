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

#ifndef TOADLET_TADPOLE_ACTION_ANIMATIONACTION_H
#define TOADLET_TADPOLE_ACTION_ANIMATIONACTION_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/action/Action.h>
#include <toadlet/tadpole/action/Animation.h>
#include <toadlet/tadpole/action/Interpolator.h>
#include <toadlet/tadpole/action/CosInterpolator.h>

namespace toadlet{
namespace tadpole{
namespace action{

class TOADLET_API AnimationAction:public Object,public Action,public AnimationListener{
public:
	TOADLET_IOBJECT(AnimationAction);

	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	AnimationAction(Animation *animation=NULL);
	virtual ~AnimationAction();

	void update(int dt);

	bool getActive() const{return mRunning;}

	void setTime(int time);
	inline int getTime() const{return mTime;}

	void setTimeScale(scalar scale);
	inline scalar getTimeScale() const{return mTimeScale;}

	void setCycling(Cycling cycling);
	inline Cycling getCycling() const{return mCycling;}

	void setStopGently(bool stop){mStopGently=stop;}
	inline bool getStopGently() const{return mStopGently;}

	void setInterpolator(Interpolator *interpolator);
	inline Interpolator *getInterpolator() const{return mInterpolator;}

	void setMinMaxValue(scalar minValue,scalar maxValue){mMinValue=minValue;mMaxValue=maxValue;}
	inline scalar getMinValue() const{return mMinValue;}
	inline scalar getMaxValue() const{return mMaxValue;}

	void addActionListener(ActionListener *listener);
	void removeActionListener(ActionListener *listener);

	void start();
	void stop();

	void setValue(scalar value);

	void attach(Animation *animation);
	void remove(Animation *animation);

	void animationExtentsChanged(Animation *animation);

protected:
	Collection<ActionListener*> mListeners;
	Collection<Animation::ptr> mAnimations;
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
}

#endif
