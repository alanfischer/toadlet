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

import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;

public class MaterialColorAnimation implements Animation{
	public MaterialColorAnimation(Material target){
		mTarget=target;
		mStart.set(target.getLightEffect().diffuse);
	}

	public MaterialColorAnimation(Material target,Color end,scalar time){
		mTarget=target;
		mStart.set(target.getLightEffect().diffuse);
		mEnd.set(end);
		mEndTime=time;
	}

	public void setTarget(Material target){mTarget=target;}
	public Material getTarget(){return mTarget;}

	public void setStart(Color start){
		mStart.set(start);
	}

	public void setEnd(Color end,scalar time){
		mEnd.set(end);
		mEndTime=time;
	}

	public void set(scalar value){
		scalar t=Math.div(value,mEndTime);

		LightEffect le=cache_set_le;
		Color.lerp(le.diffuse,mStart,mEnd,t);
		le.ambient.set(le.diffuse);
		le.specular.set(le.diffuse);
		mTarget.setLightEffect(le);
	}

	public scalar getMin(){return 0;}
	public scalar getMax(){return mEndTime;}

	public void attached(AnimationController controller){}
	public void removed(AnimationController controller){}

	protected Material mTarget=null;
	protected Color mStart=new Color();
	protected Color mEnd=new Color();
	protected scalar mEndTime=0;
	
	protected LightEffect cache_set_le=new LightEffect();
}
