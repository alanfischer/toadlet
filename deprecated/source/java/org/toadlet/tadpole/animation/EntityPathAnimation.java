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

public class EntityPathAnimation implements Animation{
	public EntityPathAnimation(Entity target){
		mTarget=target;
	}

	public void setTarget(Entity target){mTarget=target;}
	public Entity getTarget(){return mTarget;}

	public void setTrack(Track track){mTrack=track;}
	public Track getTrack(){return mTrack;}

	public void set(scalar value){
		KeyFrame[] f1=cache_set_f1,f2=cache_set_f2;
		scalar t=mTrack.getKeyFramesAtTime(value,f1,f2,mHint,0);

		Vector3 translate=cache_set_translate.reset();
		Math.lerp(translate,f1[0].translate,f2[0].translate,t);

		Quaternion rotate=cache_set_rotate.reset();
		Math.slerp(rotate,f1[0].rotate,f2[0].rotate,t);
		Matrix3x3 rotateMatrix=cache_set_rotateMatrix.reset();
		Math.setMatrix3x3FromQuaternion(rotateMatrix,rotate);

		mTarget.setTranslate(translate);
		mTarget.setRotate(rotateMatrix);
	}

	public scalar getMin(){
		return 0;
	}
	
	public scalar getMax(){
		return mTrack.length;
	}

	public void attached(AnimationController controller){}
	public void removed(AnimationController controller){}

	protected Entity mTarget=null;
	protected Track mTrack=null;
	protected int[] mHint=new int[1];

	protected Vector3 cache_set_translate=new Vector3();
	protected Quaternion cache_set_rotate=new Quaternion();
	protected Matrix3x3 cache_set_rotateMatrix=new Matrix3x3();
	protected KeyFrame[] cache_set_f1=new KeyFrame[]{new KeyFrame()},cache_set_f2=new KeyFrame[]{new KeyFrame()};
}
