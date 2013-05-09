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

#include <toadlet/tadpole/action/TransformAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

TransformAnimation::TransformAnimation(Transform::ptr target):
	//mTarget,
	//mTrack,
	mUseLookAt(false),
	//mLookAt,
	mHint(0),
	mValue(0)
{
	mTarget=target;
}

void TransformAnimation::setTarget(Transform::ptr target){
	mTarget=target;
}

void TransformAnimation::setTrack(Track::ptr track){
	mTrack=track;
}

void TransformAnimation::setLookAt(const Vector3 &lookAt){
	mLookAt.set(lookAt);
	mUseLookAt=true;
}

void TransformAnimation::setValue(scalar value){
	int f1=-1,f2=-1;
	mValue=value;
	scalar t=mTrack->getKeyFramesAtTime(mValue,f1,f2,mHint);

	const VertexBufferAccessor &vba=mTrack->getAccessor();

	Vector3 t1,t2,translate;
	vba.get3(f1,0,t1);
	vba.get3(f2,0,t2);
	Math::lerp(translate,t1,t2,t);

	if(mUseLookAt){
		Matrix4x4 lookAt;
		Math::setMatrix4x4FromLookAt(lookAt,translate,mLookAt,Math::Z_UNIT_VECTOR3,true);
		Matrix3x3 rotate;
		Math::setMatrix3x3FromMatrix4x4(rotate,lookAt);

		mTarget->setRotate(rotate);
		mTarget->setTranslate(translate);
	}
	else{
		Quaternion r1,r2,rotate;
		vba.get4(f1,1,r1);
		vba.get4(f2,1,r2);
		Math::slerp(rotate,r1,r2,t);

		mTarget->setRotate(rotate);
		mTarget->setTranslate(translate);
	}
}

}
}
}
