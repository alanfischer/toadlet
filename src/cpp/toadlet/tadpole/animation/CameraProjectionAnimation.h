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

#ifndef TOADLET_TADPOLE_ANIMATION_CAMERAPROJECTIONANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_CAMERAPROJECTIONANIMATION_H

#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class TOADLET_API CameraProjectionAnimation:public Animation{
public:
	TOADLET_SHARED_POINTERS(CameraProjectionAnimation);

	CameraProjectionAnimation(node::CameraNode::ptr target);
	virtual ~CameraProjectionAnimation(){}

	void setTarget(node::CameraNode::ptr target);
	inline node::CameraNode::ptr getTarget() const{return mTarget;}

	// HACK: Until we introduce a proper ProjectionKeyframe, this will have to do
	void setStart(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard);
	void setEnd(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard,scalar time);

	void set(scalar value);

	scalar getMin() const{return 0;}
	scalar getMax() const{return mEndTime;}

protected:
	node::CameraNode::ptr mTarget;
	scalar mStartLeft,mStartRight,mStartBottom,mStartTop,mStartNear,mStartFar;
	scalar mEndLeft,mEndRight,mEndBottom,mEndTop,mEndNear,mEndFar;
	scalar mEndTime;
};

}
}
}

#endif
