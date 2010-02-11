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

import org.toadlet.egg.*;
import org.toadlet.egg.Error;
import org.toadlet.tadpole.*;
import org.toadlet.tadpole.entity.*;

public class SkeletonAnimation implements Animation{
	public SkeletonAnimation(){}

	public void setTarget(MeshEntitySkeleton target){
		mTarget=target;
	}

	public MeshEntitySkeleton getTarget(){return mTarget;}

	public void setSequenceIndex(int sequenceIndex){
		mSequenceIndex=sequenceIndex;

		if(mController!=null){
			mController.extentsChanged();
		}
	}
	
	public int getSequenceIndex(){return mSequenceIndex;}

	public void set(scalar value){
		if(mTarget!=null){
			mTarget.updateBones(mSequenceIndex,value);
		}
	}

	public scalar getMin(){return 0;}
	public scalar getMax(){
		if(mTarget!=null){
			return mTarget.getSkeleton().sequences.get(mSequenceIndex).length;
		}
		else{
			return 0;
		}
	}

	public void attached(AnimationController controller){
		if(mController!=null){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"cannot attach animation to more than one controller");
			return;
		}

		mController=controller;
	}
	
	public void removed(AnimationController controller){
		mController=null;
	}

	protected AnimationController mController=null;
	protected MeshEntitySkeleton mTarget=null;
	protected int mSequenceIndex=0;
}
