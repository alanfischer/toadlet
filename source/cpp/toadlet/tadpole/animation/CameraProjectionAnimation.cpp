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

#include <toadlet/tadpole/animation/CameraProjectionAnimation.h>

namespace toadlet{
namespace tadpole{
namespace animation{

CameraProjectionAnimation::CameraProjectionAnimation(CameraNode::ptr target)
	//mTarget
{
	mTarget=target;
}

void CameraProjectionAnimation::setTarget(CameraNode::ptr target){
	mTarget=target;
}

void CameraProjectionAnimation::setStart(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard){
	mStartLeft=left;
	mStartRight=right;
	mStartBottom=bottom;
	mStartTop=top;
	mStartNear=neard;
	mStartFar=fard;
}

void CameraProjectionAnimation::setEnd(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard,scalar length){
	mEndLeft=left;
	mEndRight=right;
	mEndBottom=bottom;
	mEndTop=top;
	mEndNear=neard;
	mEndFar=fard;

	mMaxValue=length;
}

void CameraProjectionAnimation::setValue(scalar value){
	mValue=value;
	scalar t=Math::div(mValue,mMaxValue);

	mTarget->setProjectionFrustum(
		Math::lerp(mStartLeft,mEndLeft,t),
		Math::lerp(mStartRight,mEndRight,t),
		Math::lerp(mStartBottom,mEndBottom,t),
		Math::lerp(mStartTop,mEndTop,t),
		Math::lerp(mStartNear,mEndNear,t),
		Math::lerp(mStartFar,mEndFar,t)
	);
}

}
}
}
