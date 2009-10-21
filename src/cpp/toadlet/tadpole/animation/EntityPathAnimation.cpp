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

#include <toadlet/tadpole/animation/EntityPathAnimation.h>
#include <toadlet/tadpole/entity/ParentEntity.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole::entity;

namespace toadlet{
namespace tadpole{
namespace animation{

EntityPathAnimation::EntityPathAnimation(Entity::ptr target):
	//mTarget,
	//mTrack,
	mHint(0)
{
	mTarget=target;
}

void EntityPathAnimation::setTarget(Entity::ptr target){
	mTarget=target;
}

void EntityPathAnimation::setTrack(Track::ptr track){
	mTrack=track;
}

void EntityPathAnimation::set(scalar value){
	const KeyFrame *f1=NULL,*f2=NULL;
	scalar t=mTrack->getKeyFramesAtTime(value,f1,f2,mHint);

	Vector3 &translate=cache_set_translate.reset();
	Math::lerp(translate,f1->translate,f2->translate,t);

	Quaternion &rotate=cache_set_rotate.reset();
	Math::slerp(rotate,f1->rotate,f2->rotate,t);
	Matrix3x3 &rotateMatrix=cache_set_rotateMatrix.reset();
	Math::setMatrix3x3FromQuaternion(rotateMatrix,rotate);

	mTarget->setTranslate(translate);
	mTarget->setRotate(rotateMatrix);
}

scalar EntityPathAnimation::getMin() const{
	return 0;
}

scalar EntityPathAnimation::getMax() const{
	return mTrack->length;
}

}
}
}
