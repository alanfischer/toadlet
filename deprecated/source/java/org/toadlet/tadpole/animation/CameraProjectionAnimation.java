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

package org.toadlet.tadpole.animation;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.tadpole.*;
import org.toadlet.tadpole.entity.*;

public class CameraProjectionAnimation implements Animation{
	public CameraProjectionAnimation(CameraEntity target){
		mTarget=target;
	}

	public void setTarget(CameraEntity target){
		mTarget=target;
	}
	
	public CameraEntity getTarget(){return mTarget;}

	// HACK: Until we introduce a proper ProjectionKeyframe, this will have to do
	public void setStart(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard){
		mStartLeft=left;
		mStartRight=right;
		mStartBottom=bottom;
		mStartTop=top;
		mStartNear=neard;
		mStartFar=fard;
	}
	
	public void setEnd(scalar left,scalar right,scalar bottom,scalar top,scalar neard,scalar fard,scalar time){
		mEndLeft=left;
		mEndRight=right;
		mEndBottom=bottom;
		mEndTop=top;
		mEndNear=neard;
		mEndFar=fard;

		mEndTime=time;
	}

	public void set(scalar value){
		scalar t=Math.div(value,mEndTime);

		mTarget.setProjectionFrustum(
			Math.lerp(mStartLeft,mEndLeft,t),
			Math.lerp(mStartRight,mEndRight,t),
			Math.lerp(mStartBottom,mEndBottom,t),
			Math.lerp(mStartTop,mEndTop,t),
			Math.lerp(mStartNear,mEndNear,t),
			Math.lerp(mStartFar,mEndFar,t)
		);
	}

	public scalar getMin(){return 0;}
	public scalar getMax(){return mEndTime;}

	public void attached(AnimationController controller){}
	public void removed(AnimationController controller){}

	protected CameraEntity mTarget;
	protected scalar mStartLeft,mStartRight,mStartBottom,mStartTop,mStartNear,mStartFar;
	protected scalar mEndLeft,mEndRight,mEndBottom,mEndTop,mEndNear,mEndFar;
	protected scalar mEndTime;
}
