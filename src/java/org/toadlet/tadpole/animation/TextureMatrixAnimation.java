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

import org.toadlet.peeper.*;

public class TextureMatrixAnimation implements Animation{
	public TextureMatrixAnimation(TextureStage target){mTarget=target;}

	public void setTarget(TextureStage target){
		mTarget=target;
		mOffset.set(target.getTextureMatrix());
	}

	public TextureStage getTarget(){return mTarget;}

	public void set(scalar value){
		Matrix4x4 matrix=cache_set_matrix;
		Math.setMatrix4x4FromZ(matrix,Math.mul(value,Math.TWO_PI));
		Math.setMatrix4x4AsTextureRotation(matrix);
		Matrix4x4 temp=cache_set_temp;
		Math.mul(temp,mOffset,matrix);
		mTarget.setTextureMatrix(temp);
	}

	public scalar getMin(){return 0;}
	public scalar getMax(){return 0;}

	public void attached(AnimationController controller){}
	public void removed(AnimationController controller){}

	protected TextureStage mTarget;
	protected Matrix4x4 mOffset=new Matrix4x4();

	protected Matrix4x4 cache_set_matrix=new Matrix4x4();
	protected Matrix4x4 cache_set_temp=new Matrix4x4();
}
