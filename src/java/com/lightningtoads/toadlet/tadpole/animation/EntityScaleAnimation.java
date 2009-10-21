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

package com.lightningtoads.toadlet.tadpole.animation;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.entity.*;

public class EntityScaleAnimation implements Animation{
	public EntityScaleAnimation(Entity target){
		mTarget=target;
		mStart.set(target.getScale());
	}

	public EntityScaleAnimation(Entity target,scalar endScale,scalar time){
		mTarget=target;
		mStart.set(target.getScale());
		mEnd.set(endScale,endScale,endScale);
		mEndTime=time;
	}

	public EntityScaleAnimation(Entity target,Vector3 end,scalar time){
		mTarget=target;
		mStart.set(target.getScale());
		mEnd.set(end);
		mEndTime=time;
	}

	public void setTarget(Entity target){mTarget=target;}
	public Entity getTarget(){return mTarget;}

	public void setStart(Vector3 start){
		mStart.set(start);
	}

	public void setEnd(Vector3 end,scalar time){
		mEnd.set(end);
		mEndTime=time;
	}

	public void set(scalar value){
		scalar t=Math.div(value,mEndTime);

		Vector3 scale=cache_set_scale;
		Math.lerp(scale,mStart,mEnd,t);
		mTarget.setScale(scale);
	}

	public scalar getMin(){return 0;}
	public scalar getMax(){return mEndTime;}

	public void attached(AnimationController controller){}
	public void removed(AnimationController controller){}

	protected Entity mTarget=null;
	protected Vector3 mStart=new Vector3();
	protected Vector3 mEnd=new Vector3();
	protected scalar mEndTime=0;

	protected Vector3 cache_set_scale=new Vector3();
}
