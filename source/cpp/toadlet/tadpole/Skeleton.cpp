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

#include <toadlet/tadpole/Skeleton.h>

namespace toadlet{
namespace tadpole{

Skeleton::Skeleton():BaseResource(){
}

Skeleton::~Skeleton(){
	destroy();
}

void Skeleton::destroy(){
	sequences.clear();
}

void Skeleton::compile(){
	int i;
	for(i=0;i<bones.size();++i){
		Bone *bone=bones[i];

		Vector3 wtbtranslation(bone->translate);
		Quaternion wtbrotation(bone->rotate);
		Bone *parentBone=bone;
		while(parentBone!=NULL){
			if(parentBone->parentIndex==-1){
				parentBone=NULL;
			}
			else{
				parentBone=bones[parentBone->parentIndex];
				Matrix3x3 rotate;
				Math::setMatrix3x3FromQuaternion(rotate,parentBone->rotate);
				Math::mul(wtbtranslation,rotate);
				Math::add(wtbtranslation,parentBone->translate);
				Math::preMul(wtbrotation,parentBone->rotate);
			}
		}

		Math::neg(wtbtranslation);
		Math::invert(wtbrotation);

		Matrix3x3 rotate;
		Math::setMatrix3x3FromQuaternion(rotate,wtbrotation);
		Math::mul(wtbtranslation,rotate);

		bone->worldToBoneTranslate.set(wtbtranslation);
		bone->worldToBoneRotate.set(wtbrotation);
	}
}

Skeleton::Bone::ptr Skeleton::getBone(const String &name){
	int i;
	for(i=0;i<bones.size();++i){
		if(bones[i]->name==name){
			return bones[i];
		}
	}
	return NULL;
}

}
}
