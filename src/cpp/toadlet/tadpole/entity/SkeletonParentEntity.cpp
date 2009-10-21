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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/entity/SkeletonParentEntity.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace entity{

SkeletonParentEntity::SkeletonParentEntity():ParentEntity(){}

void SkeletonParentEntity::setSkeleton(MeshEntitySkeleton::ptr skeleton){
	mSkeleton=skeleton;

	setReceiveUpdates(mSkeleton!=NULL);
}

bool SkeletonParentEntity::attach(Entity *entity,int bone){
	bool result=super::attach(entity);
	if(result){
		mChildrenBones.add(bone);
	}
	return result;
}

bool SkeletonParentEntity::remove(Entity *entity){
	int i;
	for(i=0;i<mChildren.size();++i){
		if(mChildren[i]==entity){
			mChildrenBones.removeAt(i);
			break;
		}
	}
	return super::remove(entity);
}

void SkeletonParentEntity::visualUpdate(int dt){
	int i;
	for(i=0;i<mChildren.size();++i){
		MeshEntitySkeleton::Bone *bone=mSkeleton->getBone(mChildrenBones[i]);
		mChildren[i]->setTranslate(bone->worldTranslate);
		mChildren[i]->setRotate(bone->worldRotateMatrix);
	}
}

}
}
}
