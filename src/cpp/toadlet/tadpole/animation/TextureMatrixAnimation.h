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

#ifndef TOADLET_TADPOLE_ANIMATION_TEXTUREMATRIXANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_TEXTUREMATRIXANIMATION_H

#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/animation/Animation.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class TextureMatrixAnimation:public Animation{
public:
	TOADLET_SHARED_POINTERS(TextureMatrixAnimation,Animation);

	TextureMatrixAnimation(peeper::TextureStage::ptr target){mTarget=target;}
	virtual ~TextureMatrixAnimation(){}

	void setTarget(peeper::TextureStage::ptr target){
		mTarget=target;
		mOffset.set(target->getTextureMatrix());
	}
	inline peeper::TextureStage::ptr getTarget() const{return mTarget;}

	void set(scalar value){
		Matrix4x4 &matrix=cache_set_matrix;
		Math::setMatrix4x4FromZ(matrix,Math::mul(value,Math::TWO_PI));
		Math::setMatrix4x4AsTextureRotation(matrix);
		Matrix4x4 &temp=cache_set_temp;
		Math::mul(temp,mOffset,matrix);
		mTarget->setTextureMatrix(temp);
	}

	scalar getMin() const{return 0;}
	scalar getMax() const{return 0;}

	void attached(AnimationController *){}
	void removed(AnimationController *){}

protected:
	peeper::TextureStage::ptr mTarget;
	Matrix4x4 mOffset;

	Matrix4x4 cache_set_matrix;
	Matrix4x4 cache_set_temp;
};

}
}
}

#endif
