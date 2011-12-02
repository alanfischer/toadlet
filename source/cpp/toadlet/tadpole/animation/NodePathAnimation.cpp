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

#include <toadlet/tadpole/animation/NodePathAnimation.h>
#include <toadlet/tadpole/node/ParentNode.h>

namespace toadlet{
namespace tadpole{
namespace animation{

NodePathAnimation::NodePathAnimation(Node::ptr target):
	//mTarget,
	//mTrack,
	mUseLookAt(false),
	//mLookAt,
	mHint(0)
{
	mTarget=target;
}

void NodePathAnimation::setTarget(Node::ptr target){
	mTarget=target;
}

void NodePathAnimation::setTrack(TransformTrack::ptr track){
	mTrack=track;
}

void NodePathAnimation::setLookAt(const Vector3 &lookAt){
	mLookAt.set(lookAt);
	mUseLookAt=true;
}

void NodePathAnimation::set(scalar value){
	const TransformKeyFrame *f1=NULL,*f2=NULL;
	scalar t=mTrack->getKeyFramesAtTime(value,f1,f2,mHint);

	Vector3 &translate=cache_set_translate.reset();
	Math::lerp(translate,f1->translate,f2->translate,t);

	if(mUseLookAt){
		Matrix4x4 lookAt;
		Math::setMatrix4x4FromLookAt(lookAt,translate,mLookAt,Math::Z_UNIT_VECTOR3,true);
		Matrix3x3 &rotateMatrix=cache_set_rotateMatrix.reset();
		Math::setMatrix3x3FromMatrix4x4(rotateMatrix,lookAt);

		mTarget->setRotate(rotateMatrix);
		mTarget->setTranslate(translate);
	}
	else{
		Quaternion &rotate=cache_set_rotate.reset();
		Math::slerp(rotate,f1->rotate,f2->rotate,t);

		mTarget->setRotate(rotate);
		mTarget->setTranslate(translate);
	}
}

scalar NodePathAnimation::getMin() const{
	return 0;
}

scalar NodePathAnimation::getMax() const{
	return mTrack->length;
}

}
}
}
